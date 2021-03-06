//	Библиотека работы arduino с ИК приёмопередатчиками
//  Примечание: Библиотека использует второй аппаратный таймер, НЕ ВЫВОДИТЕ СИГНАЛЫ ШИМ НА 3 ИЛИ 11 ВЫВОД!

//	Подключение библиотеки:
//	#include <iarduino_IR.h>
//	iarduino_IR IR(вывод ИК приёмника [, вывод ИК передатчика [, флаг инверсии сигнала для ИК передатчика]]);
//	                                         если нет необходимости передавать сигналы (нет ИК передатчика), то 2 и 3 параметры можно не указывать
//	                                         флаг инверсии сигнала для ИК передатчика по умолчанию равен false - без инверсии, то есть при отсутствии сигнала, на выводе для ИК передатчика устанавливается уровень логического «0»
//	
//	В библиотеке реализованы 4 функции:
//	Функция begin     Назначение:            Инициализация ИК приёмопередатчика
//	                  Синтаксис:             begin();
//	                  Возвращаемые значения: Нет
//	                  Примечание:            Вызывается 1 раз в начале кода. Сама определяет нужна или нет инверсия сигнала от ИК датчика
//	
//	Функция check     Назначение:            Определение и расшифровка принятых данных
//	                  Синтаксис:             check([ bool флаг_реагирования_на_сигналы_удержания_кнопки_пульта]);
//	                  Возвращаемые значения: bool true - при наличии принятого сигнала, или false при его отсутствии
//	                  Примечание:            После возврата true, данные можно прочитать из переменных - data, length, coding, frequency, data_repeat и length_repeat
//	                                         Флаг реагирования на сигналы удержания кнопки пульта по умолчанию равен false - не реагировать, то есть при удержании кнопки на пульте, функция вернет true только 1 раз, в момент нажатия
//	                                         Если указать параметр true, то функция будет возвращать true пока удерживается кнопка на пульте
//	
//	Функция send      Назначение:            Отправка данных на ИК-светодиод (если его вывод указан при объявлении переменной IR)
//	                  Синтаксис:             send( uint32_t данные [, bool флаг_отправки_повторов]);
//	                  Возвращаемые значения: Нет
//	                  Примечание:            Флаг отправки повторов по умолчанию равен false - отправить только один информационный пакет
//	                                         Если указать флаг отправки повторов равный true, то функция подавляет дребезг при нажатии кнопки и отправляет пакеты повторов при её удержании
//	                                         Если не указан протокол передачи данных, то отправка происходит по последнему определённому функцией check протоколу передачи данных
//	
//	Функция protocol  Назначение:            Установка/чтение/сброс протокола передачи данных (зависит от наличия и типа параметра функции)
//	                  Синтаксис:             protocol( char* "строка" );          - указание протокола в виде строки из 25 символов + символ конца строки
//	                  Синтаксис:             protocol();                          - чтение   протокола в виде строки из 25 символов + символ конца строки
//	                  Синтаксис:             protocol(false);                     - отмена ранее указанного протокола
//	                  Возвращаемые значения: При чтении протокола возвращается строка из 25 символов + символ конца строки
//	                  Примечание:            Если протокол читается,    то возвращаемая строка соответствует протоколу передачи последних успешно принятых функцией check данных
//	                                         Если протокол указывается, то дальнейшие действия приём/передача будут определяться указанным протоколом передачи данных
//
//	Переменные:      Указанные переменные доступны для чтения после возврата true функцией check
//	coding           - кодировка                    (целое число от 0 до 63)
//	                   значение переменной можно сравнивать с константами:
//	                   coding==IR_UNDEFINED         тип кодирования не определён
//	                   coding==IR_PAUSE_LENGTH      кодирование длинной паузы
//	                   coding==IR_PULSE_LENGTH      кодирование шириной импульса (ШИМ)
//	                   coding==IR_ONE_PULSE_LENGTH  кодирование шириной одного импульса
//	                   coding==IR_BIPHASIC          бифазное кодирование
//	                   coding==IR_BIPHASIC_INV      бифазное кодирование с инверсными битами
//	                   coding==IR_NRC               NOKIA	бифазное кодирование с сигналом старт, пакеты повтора идентичны, первый и последний пакеты специальные
//	                   coding==IR_RS5               PHILIPS	бифазное кодирование с битом toggle (третий бит в пакете), пакеты повтора идентичны первому
//	                   coding==IR_RS5X              PHILIPS	бифазное кодирование с битом toggle (второй бит в пакете), пакеты повтора идентичны первому
//	                   coding==IR_RS6               PHILIPS	бифазное кодирование с битом toggle (пятый бит в пакете) и сигналом старт, пакеты повтора идентичны первому
//	frequency        - несущая частота в кГц        (целое число от 0 до 63)
//	                   данной переменной можно присвоить свою частоту передачи данных, это может увеличить радиус действия при передаче команд функцией send
//	                   если указать частоту 0 кГц, то функция send будет отправлять не модулированный сигнал
//	data             - принятые данные              (целое 32 битное число)  только для чтения
//	length           - количество принятых бит      (целое число от 0 до 32) только для чтения, позволяет отличить данные 0x00FF от данных 0xFF
//	data_repeat      - принятые данные из пакета повтора (целое 32 битное число) только для чтения
//	length_repeat    - количество принятых бит из пакета повтора (целое число от 0 до 32) только для чтения, позволяет отличить данные 0x00FF от данных 0xFF
//	key_press        - флаг сообщающий, что функция check вызванная с паретром rtue вернула положительный результат на информационный пакет данных, а не на пакеты повторов
//	
//	Примечание:      Библиотека использует второй аппаратный таймер, не выводите сигналы ШИМ на 3 или 11 вывод.
//	                 Строка протокола передачи данных это 25 символов, каждый символ занимает 1 байт.
//	                   для того чтобы не завесить от кодировок и иметь возможность скопировать строку в скетч с монитора, я "пожертвовал" двумя первыми битами каждого символа, они всегда равны "01"
//	                   таким образом строка может состоять только из символов латинского алфавита и символов @[]{}\|^`_~
//	                   оставшиеся 6 бит каждого символа являются информационными и несут информацию о протоколе передачи данных:
//	                   - символы: 1=coding, 2=frequency, 3=length, 4=length_repeat, 12=позиции_бита_рестарт, 13=макс_длит_импульсов_(мкс/50), 14=мин_длит_импульсов_(мкс/50), 15=макс_длит_пауз_(мкс/50), 16=мин_длит_пауз_(мкс/50).                                                Старшие два бита равные "01" у этих символов заменяются на "00".
//	                   - символы: 5=длит_пауз_между_пакетами_(мс), 6=длит_импульса_старт_(мкс/50), 7=длит_паузы_старт_(мкс/50), 8=длит_импульса_стоп_(мкс/50), 9=длит_паузы_стоп_(мкс/50), 10=длит_импульса_рестарт_или_toggle_(мкс/50), 11=длит_паузы_рестарт_или_toggle_(мкс/50). Старшие два бита равные "01" у этих символов заменяются на два бита из символов 22,23,24.
//	                   - символы: 17=старшие_6_бит_пакета_повтора, 18=вторые_6_бит_пакета_повтора, 19=третие_6_бит_пакета_повтора, 20=четвёртые_6_бит_пакета_повтора, 21=пятые_6_бит_пакета_повтора, последние (младшие) 2 бита пакета повтора находятся в 22 символе.              Старшие два бита равные "01" у этих символов не учитываются.
//	                   - символы: 22,23,24 содержат по три пары бит для предыдущих символов, а символ 25 содержит флаги наличия сигналов старт, рестарт, toggle и стоп.
//	                 Переменная data (data_repeat) содержит все биты пакета, за исключением сигналов старт, стоп, рестарт и бита toggle.
//	                 Если сигнал старт (стоп) по своей форме идентичен биту 1 (0), то он расценивается как бит 1 (0) и включается в переменную data (data_repeat)

#ifndef iarduino_IR_h
#define iarduino_IR_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
								
#define	IR_UNDEFINED		0		//			тип кодирования не определён
#define	IR_PAUSE_LENGTH		1		//			кодирование длинной паузы
#define	IR_PULSE_LENGTH		2		//			кодирование шириной импульса (ШИМ)
#define	IR_ONE_PULSE_LENGTH	3		//			кодирование шириной одного импульса
#define	IR_BIPHASIC			4		//			бифазное кодирование
#define	IR_BIPHASIC_INV		5		//			бифазное кодирование с инверсными битами
#define	IR_NRC				10		//	NOKIA	бифазное кодирование с сигналом старт, пакеты повтора идентичны, первый и последний пакеты специальные
#define	IR_RS5				11		//	PHILIPS	бифазное кодирование с битом toggle (третий бит в пакете), пакеты повтора идентичны первому
#define	IR_RS5X				12		//	PHILIPS	бифазное кодирование с битом toggle (второй бит в пакете), пакеты повтора идентичны первому
#define	IR_RS6				13		//	PHILIPS	бифазное кодирование с битом toggle (пятый бит в пакете) и сигналом старт, пакеты повтора идентичны первому

#define	IR_INTERVAL_PACK	7		//	минимальный интервал между повторными пакетами в мс
#define	IR_INTERVAL_PRESS	200		//	минимальный интервал между нажатиями клавиш в мс

class iarduino_IR{
	public:			iarduino_IR	(uint8_t, uint8_t=255, bool=false);	//	подключение (№ вывода подключённого к датчику, № вывода подключённого к светодиоду, инверсия сигнала для светодиода)
	/**	пользовательские функции **/
		void		begin		();									//	инициализация ИК датчика			(без параметра)
		bool		check		(bool=false);						//	проверка наличия принятого пакета	(реагировать на пакеты повтора)
		void		send		(uint32_t, bool=false);				//	отправить пакет данных на светодиод	(данные, флаг передачи повторных пакетов при удержании)
		void		protocol	(bool);								//	сбросить протокол передачи данных	(false)
		void		protocol	(char*);							//	указать  протокол передачи данных	(строка протокола)
		char*		protocol	();									//	получить протокол передачи данных	(без параметра)
	/**	переменные доступные для пользователя **/
		uint8_t		coding							=	0;			//	кодировка (определённый или установленный протокол передачи данных)
		uint8_t		frequency						=	37;			//	несущая частота для передачи сигнала
		uint32_t	data							=	0;			//	переменная для чтения данных
		uint8_t		length							=	0;			//	переменная для чтения длинны данных (количество информационных бит)
		uint32_t	data_repeat						=	0;			//	переменная для чтения данных пакета повтора
		uint8_t		length_repeat					=	0;			//	переменная для чтения длинны данных (количество информационных бит)
		bool		key_press                       =   0;          //  флаг указвающий на то, что кнопка пульта нажимается а не удерживается
	private:
	/**	внутренние функции **/
		void		IR_func_TIMER2_SETREG(uint32_t);				//	установка таймера2					(частота прерываний в кГц)
		uint8_t		IR_func_PROTOCOL();								//	определение протокола передачи данных
		bool		IR_func_DECODE(uint8_t);						//	раскодирование в IR_uint_DATA		(номер пакета начиная с 0)
		bool		IR_func_DECODE_BIPHASIC(uint8_t, bool);			//	декодирование бифазного кода		(номер пакета начиная с 0, true если _/¯ = 1  ¯\_ = 0   false если _/¯ = 0  ¯\_ = 1 )
		void		IR_func_DECODE_BIPHASIC_SHIFT(bool);			//	сдвиг данных на 1 бит				(true - влево, false - вправо)
		bool		IR_func_CHECK_NRC();							//	проверка протокола NRC
		uint32_t	IR_func_CREATE_PACK_REPEAT(bool, uint32_t=0);	//	создание повторного пакета 			(0-для протокола/1-для отправки на светодиод, отправляемые на светодиод данные)
		bool		IR_func_COMPARE(uint8_t, uint8_t, uint8_t);		//	сравнение чисел с указанием допуска	(число, число, допуск±)
		void		IR_func_DELAY(bool, uint32_t);					//	передача сигнала на светодиод		(0/1, удержание)
		void		IR_func_SEND(uint32_t, uint8_t);				//	передача пакета  на светодиод		(данные, длинна)
	/**	внутренние переменные **/
		uint8_t		IR_var_I						=	0;			//	переменная
		uint8_t		IR_var_J						=	0;			//	переменная
		uint8_t		IR_var_K						=	0;			//	переменная
		char		IR_char_PROTOCOL[26];							//	строка с подробной информацией о протоколе
		bool		IR_flag_SET_PROTOCOL			=	0;			//	флаг установки протокола передачи данных пользователем
		bool		IR_flag_START					=	0;			//	флаг наличия сигнала старт
		bool		IR_flag_STOP					=	0;			//	флаг наличия сигнала стоп
		bool		IR_flag_TOGGLE					=	0;			//	флаг наличия бита toggle/рестарт
		bool		IR_flag_TOGGLE_SEND				=	0;			//	флаг последнего состояния бита toggle
		bool		IR_flag_EVEN_SEND				=	0;			//	чётность последнего отправленного пакета
		uint8_t		IR_uint_BIT_PULSE_MAX			=	0;			//	максимальная длительность импульса в мкс/50
		uint8_t		IR_uint_BIT_PULSE_MIN			=	0;			//	минимальная  длительность импульса в мкс/50
		uint8_t		IR_uint_BIT_PAUSE_MAX			=	0;			//	максимальная длительность паузы в мкс/50
		uint8_t		IR_uint_BIT_PAUSE_MIN			=	0;			//	минимальная  длительность паузы в мкс/50
		uint8_t		IR_uint_START_PULSE				=	0;			//	длительность импульса сигнала старт в мкс/50
		uint8_t		IR_uint_START_PAUSE				=	0;			//	длительность паузы сигнала старт в мкс/50
		uint8_t		IR_uint_STOP_PULSE				=	0;			//	длительность импульса сигнала стоп в мкс/50
		uint8_t		IR_uint_STOP_PAUSE				=	0;			//	длительность паузы сигнала стоп в мкс/50
		uint8_t		IR_uint_TOGGLE_PULSE			=	0;			//	длительность импульса бита toggle/рестарт в мкс/50
		uint8_t		IR_uint_TOGGLE_PAUSE			=	0;			//	длительность паузы бита toggle/рестарт в мкс/50
		uint8_t		IR_uint_TOGGLE_POSITION			=	0;			//	позиция бита toggle/рестарт в пакете
		uint8_t		IR_uint_TOGGLE_PULSE_POS		=	0;			//	позиция импульса бита toggle/рестарт в пакете
		uint8_t		IR_uint_TOGGLE_PAUSE_POS		=	0;			//	позиция паузы бита toggle/рестарт в пакете
		uint8_t		IR_uint_TOGGLE_PULSE_SUM		=	0;			//	кол-во битов toggle/рестарт в пакете
		uint8_t		IR_uint_TOGGLE_PAUSE_SUM		=	0;			//	кол-во битов toggle/рестарт в пакете
		uint8_t		IR_uint_PACK_PAUSE				=	0;			//	пауза между 1 и 2 пакетами в мс
		uint32_t	IR_uint_PACK_REPEAT				=	0;			//	значение или биты инверсии для отправки пакета повтора
		uint8_t		IR_uint_PACK_REPEAT_TYPE		=	0;			//	тип повторного пакета (0-нет, 1-с инверсными битами, 2-идентичен информационному, 3-уникален)
		uint8_t		IR_uint_LENGTH					=	0;			//	длинна данных 1 или 2 пакета в битах (временная переменная, её значение присваивается переменным length или length_repeat)
		uint32_t	IR_uint_DATA					=	0;			//	значение данных в 1 или 2 пакете     (временная переменная, её значение присваивается переменным data или data_repeat)
		uint32_t	IR_uint_DATA_PREVIOUS			=	0;			//	значение данных отправленных последними функцией send
		uint16_t	IR_mass_BIPHASIC_BIN[4];						//	массив с данными бифазного кода в двоичном виде
		uint8_t		IR_uint_PACK_LEN[2];							//	длинна массива с данными 1 и 2 пакета
		uint8_t		IR_mass_PACK[2][68];							//	массив с данными 1 и 2 пакета (длительность импульсов и паузы, в мкс/50)
};

class iarduino_IR_volatile_variable_class{
	public:
	/**	переменные изменяемые в прерываниях **/
		volatile	uint8_t		IR_pins_NUM_INPUT	=	0;			//	номер вывода к которому подключён ИК приёмник
		volatile	uint8_t		IR_pins_NUM_OUTPUT	=	0;			//	номер вывода к которому подключён светодиод
		volatile	uint8_t		IR_uint_READ_STATUS	=	0;			//	состояние приёма пакетов
																	//	0 - нет пакетов
																	//	1 - принимается первый пакет	3 - принимается второй пакет
																	//	2 - принят первый пакет			4 - принят второй пакет
																	//	5 - принимаются или приняты следующие пакеты
																	//	6 - пауза после последнего пакета более 200мс
		volatile	bool		IR_pins_SEND_STATUS	=	0;			//	состояние передачи несущей частоты (не передаётся/передаётся)
		volatile	bool		IR_pins_READ		=	0;			//	состояние на выводе к которому подключён ИК приёмник
		volatile	bool		IR_pins_SEND		=	0;			//	состояние на выводе к которому подключён светодиод (0/1)
		volatile	bool		IR_flag_CHECK		=	0;			//	флаг выполнения функции check
		volatile	bool		IR_flag_KEY_PRESS	=	0;			//	флаг выполнения функции check с имитированным паратром false
		volatile	bool		IR_flag_READ_REPEAT	=	0;			//	флаг наличия повторного пакета старше второго (удерживается клавиша на пульте)
		volatile	bool		IR_flag_PULSE		=	0;			//	флаг состояния сигнала в данный момент времени (1-PULSE/0-PAUSE)
		volatile	bool		IR_flag_READ_INVERT	=	0;			//	флаг инвертирования сигналов принимаемых от ИК приёмника
		volatile	bool		IR_flag_SEND_INVERT	=	0;			//	флаг инвертирования сигналов передаваемых на светодиод
		volatile	bool		IR_flag_SEND		=	0;			//	флаг передачи данных
		volatile	uint16_t	IR_uint_PACK_PAUSE	=	0;			//	пауза между 1 и 2 пакетами
		volatile	uint8_t		IR_uint_CALL_PAUSE	=	0;			//	пауза между вызовами функции send (в мкс/50)
		volatile	uint16_t	IR_uint_PACK_LENGTH	=	0;			//	длинна сигнала или паузы принимаемой в данный момент времени (в мкс/50)
		volatile	uint8_t		IR_uint_PACK_INDEX	=	0;			//	индекс в массиве с данными, в который сейчас записывается время
		volatile	uint8_t		IR_uint_PACK_NUM	=	0;			//	номер массива в который записывается пакет
		volatile	uint8_t		IR_uint_PACK_LEN[2]	=	{0,0};		//	длинна   массива с данными 1 и 2 пакета
		volatile	uint8_t		IR_mass_PACK[2][68];				//	массив с данными 1 и 2 пакета (длительность импульсов и паузы, в мкс/50)
};

#endif