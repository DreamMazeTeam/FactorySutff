#pragma once

#ifndef ENCODER_H
#define ENCODER_H

#ifndef ARDUINO_IDE
#include <Arduino.h>
#endif

#define ENC_NO_BUTTON -1	// константа для работы без пина
#define TYPE1 0			// полушаговый энкодер
#define TYPE2 1			// полношаговый
#define NORM 0			// направление вращения обычное
#define REVERSE 1		// обратное
#define MANUAL 0		// нужно вызывать функцию tick() вручную
#define AUTO 1			// tick() входит во все остальные функции и опрашивается сама!
#define HIGH_PULL 0		// внутренняя подтяжка к питанию (pinMode INPUT_PULLUP)
#define LOW_PULL 1		// внешняя подтяжка к GND (pinMode INPUT)

// закомментируй строку, чтобы полностью убрать отработку кнопки из кода
//#define ENC_WITH_BUTTON

// тип подключения энкодера по умолчанию (LOW_PULL или HIGH_PULL)
//#define DEFAULT_ENC_PULL LOW_PULL
#define DEFAULT_ENC_PULL HIGH_PULL

// тип подключения кнопки энкодера по умолчанию (LOW_PULL или HIGH_PULL)
//#define DEFAULT_BTN_PULL LOW_PULL
#define DEFAULT_BTN_PULL HIGH_PULL

// алгоритмы опроса энкодера (раскомментировать нужный)
//#define FAST_ALGORITHM		// тик 10 мкс, быстрый, не справляется с люфтами
//#define BINARY_ALGORITHM	// тик 14 мкс, лучше справляется с люфтами
#define PRECISE_ALGORITHM	// тик 16 мкс, медленнее, но работает даже с убитым энкодером (по мотивам https://github.com/mathertel/RotaryEncoder)

// настройка антидребезга энкодера, кнопки, таймаута удержания и таймаута двойного клика
#define ENC_DEBOUNCE_TURN 1
#define ENC_DEBOUNCE_BUTTON 1
#define ENC_HOLD_TIMEOUT 700
#define ENC_DOUBLE_TIMEOUT 300

#pragma pack(push,1)
struct EncoderFlags {	
	bool hold_flag: 1;
	bool butt_flag: 1;
	bool turn_flag: 1;
	bool isTurn_f: 1;
	bool isPress_f: 1;
	bool isRelease_f: 1;
	bool isHolded_f: 1;
	bool isFastR_f: 1;
	bool isFastL_f: 1;
	bool enc_tick_mode: 1;
	bool enc_type: 1;
	bool use_button : 1;
	bool extCLK : 1;
	bool extDT : 1;
	bool extSW : 1;
	bool invBtn : 1;
	bool isSingle_f : 1;
	bool isDouble_f : 1;
	bool countFlag : 1;
	bool doubleFlag : 1;
	bool doubleAllow : 1;
};
#pragma pack(pop)

class Encoder {
public:
	Encoder();								// для непривязанного к пинам энкодера
	Encoder(uint8_t clk, uint8_t dt, int8_t sw = -1, bool type = false); // CLK, DT, SW, тип (TYPE1 / TYPE2) TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип
	
	void tick();							// опрос энкодера, нужно вызывать постоянно или в прерывании
	void tick(bool clk, bool dt, bool sw = 0);	// опрос "внешнего" энкодера
	void setType(bool type);				// TYPE1 / TYPE2 - тип энкодера TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип
	void setPinMode(bool mode);				// тип подключения энкодера, подтяжка HIGH_PULL (внутренняя) или LOW_PULL (внешняя на GND)
	void setBtnPinMode(bool mode);			// тип подключения кнопки, подтяжка HIGH_PULL (внутренняя) или LOW_PULL (внешняя на GND)
	void setTickMode(bool tickMode); 		// MANUAL / AUTO - ручной или автоматический опрос энкодера функцией tick(). (по умолчанию ручной)
	void setDirection(bool direction);		// NORM / REVERSE - направление вращения энкодера
	void setFastTimeout(uint16_t timeout);	// установка таймаута быстрого поворота
	
	bool isTurn();						// возвращает true при любом повороте, сама сбрасывается в false
	bool isRight();						// возвращает true при повороте направо, сама сбрасывается в false
	bool isLeft();						// возвращает true при повороте налево, сама сбрасывается в false
	bool isRightH();						// возвращает true при удержании кнопки и повороте направо, сама сбрасывается в false
	bool isLeftH();						// возвращает true при удержании кнопки и повороте налево, сама сбрасывается в false
	bool isFastR();						// возвращает true при быстром повороте
	bool isFastL();						// возвращает true при быстром повороте
	
	bool isPress();						// возвращает true при нажатии кнопки, сама сбрасывается в false
	bool isRelease();					// возвращает true при отпускании кнопки, сама сбрасывается в false
	bool isClick();						// возвращает true при нажатии и отпускании кнопки, сама сбрасывается в false
	bool isHolded();						// возвращает true при удержании кнопки, сама сбрасывается в false
	bool isHold();						// возвращает true при удержании кнопки, НЕ СБРАСЫВАЕТСЯ
	bool isSingle();						// возвращает true при одиночном клике (после таймаута), сама сбрасывается в false
	bool isDouble();						// возвращает true при двойном клике, сама сбрасывается в false
	
	EncoderFlags flags;

	uint8_t _fast_timeout = 50;				// таймаут быстрого поворота
	uint8_t prevState = 0;
	uint8_t encState = 0;	// 0 не крутился, 1 лево, 2 право, 3 лево нажат, 4 право нажат
	uint32_t debounce_timer = 0, fast_timer = 0;
	uint8_t _CLK = 0, _DT = 0, _SW = 0;
	bool turnFlag = false, extTick = false, SW_state = false;
};


#endif