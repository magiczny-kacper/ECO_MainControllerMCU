/*
 * nextion.h
 *
 *  Created on: Jul 18, 2020
 *      Author: Kacper
 */

#ifndef SRC_NEXTION_NEXTION_H_
#define SRC_NEXTION_NEXTION_H_



#include <stdbool.h>

//Return Codes dependent on bkcmd value being greater than 0
#define NEXTION_INVALID_INSTRUCTION_CODE		0x00	//0x00 0xFF 0xFF 0xFF
#define NEXTION_INSTRUCTION_SUCCESSFUL_CODE		0x01	//0x01 0xFF 0xFF 0xFF
#define NEXTION_INVALID_COMPONENT_ID_CODE		0x02	//0x02 0xFF 0xFF 0xFF
#define NEXTION_INVALID_PAGE_ID_CODE			0x03	//0x03 0xFF 0xFF 0xFF
#define NEXTION_INVALID_PICTURE_ID_CODE			0x04	//0x04 0xFF 0xFF 0xFF
#define NEXTION_INVALID_FONT_ID					0x05	//0x05 0xFF 0xFF 0xFF
#define NEXTION_INVALID_FILE_OPERAT_CODE		0x06	//0x06 0xFF 0xFF 0xFF
#define NEXTION_INVALID_CRC_CODE				0x09	//0x09 0xFF 0xFF 0xFF
#define NEXTION_INVALID_BAUDRATE_CODE			0x11	//0x11 0xFF 0xFF 0xFF
#define NEXTION_INVALID_WAVEFORM_ID_CHANNEL_CODE	0x12	//0x12 0xFF 0xFF 0xFF
#define NEXTION_INVALID_VAR_NAME_ATRRIBUTE_CODE		0x1A	//0x1A 0xFF 0xFF 0xFF
#define NEXTION_INVALID_VAR_OPERATION_CODE		0x1B	//0x1B 0xFF 0xFF 0xFF
#define NEXTION_FAILED_ASSIGNMENT_CODE			0x1C	//0x1C 0xFF 0xFF 0xFF
#define NEXTION_FAILED_EEPROM_OPER_CODE			0x1D	//0x1D 0xFF 0xFF 0xFF
#define NEXTION_INVALID_QUANT_OF_PARAM_CODE		0x1E	//0x1E 0xFF 0xFF 0xFF
#define NEXTION_FAILED_IO_OPERAT_CODE			0x1F	//0x1F 0xFF 0xFF 0xFF
#define NEXTION_INVALID_ESCAPE_CHAR_CODE		0x20	//0x20 0xFF 0xFF 0xFF
#define NEXTION_TOO_LONG_VAR_NAME_CODE			0x23	//0x23 0xFF 0xFF 0xFF

//Return Codes not affected by bkcmd value, valid in all cases
#define NEXTION_STARTUP_CODE 				0x00	//0x00 0x00 0x00 0xFF 0xFF 0xFF
#define NEXTION_SERIAL_BUFFER_OVERFLOW      0x24	//0x24 0xFF 0xFF 0xFF
#define NEXTION_RETURN_CODE_TOUCH_EVENT     0x65	//0x65 0x00 0x01 0x01 0xFF 0xFF 0xFF
#define NEXTION_CURRENT_PAGE_NUMBER			0x66	//0x66 0x01 0xFF 0xFF 0xFF
#define NEXTION_TOUCH_COORDINATE_AWAKE		0x67	//0x67 0x00 0x7A 0x00 0x1E 0x01 0xFF 0xFF 0xFF
#define NEXTION_TOUCH_COORDINATE_SLEEP		0x68	//0x68 0x00 0x7A 0x00 0x1E 0x01 0xFF 0xFF 0xFF
#define NEXTION_STRING_DATA_ENCLOSED		0x70	//0x70 0x61 0x62 0x31 0x32 0x33 0xFF 0xFF 0xFF
#define NEXTION_NUMERIC_DATA_ENCLOSED		0x71	//0x71 0x01 0x02 0x03 0x04 0xFF 0xFF 0xFF
#define NEXTION_AUTO_ENTERED_SLEEP_MODE		0x86 	//0x86 0xFF 0xFF 0xFF
#define NEXTION_AUTO_WAKE_FROM_SLEEP		0x87	//0x87 0xFF 0xFF 0xFF
#define NEXTION_READY						0x88	//0x88 0xFF 0xFF 0xFF
#define NEXTION_START_MICRO_SD_UPGRADE		0x89	//0x89 0xFF 0xFF 0xFF
#define NEXTION_TRANSPARENT_DATA_FINISHED	0xFD	//0xFD 0xFF 0xFF 0xFF
#define NEXTION_TRANSPARENT_DATA_READY		0xFE	//0xFE 0xFF 0xFF 0xFF

//Color Constant
#define Nextion_BLACK_CONSTANT		"BLACK"	//0
#define Nextion_BLUE_CONSTANT		"BLUE"  //31
#define Nextion_BROWN_CONSTANT		"BROWN" //48192
#define Nextion_GREEN_CONSTANT		"GREEN" //2016
#define Nextion_YELLOW_CONSTANT		"YELLOW"//65504
#define Nextion_RED_CONSTANT	    "RED"	//63488
#define Nextion_GRAY_CONSTANT		"GRAY"	//33840
#define Nextion_WHITE_CONSTANT		"WHITE"	//65535

//Color Value
#define Nextion_BLACK_565_VALUE			0
#define Nextion_BLUE_565_VALUE			31
#define Nextion_BROWN_565_VALUE		  	48192
#define Nextion_GREEN_565_VALUE			2016
#define Nextion_YELLOW_565_VALUE		65504
#define Nextion_RED_565_VALUE		    63488
#define Nextion_GRAY_565_VALUE			33840
#define Nextion_WHITE_565_VALUE			65535

//Baudrate
typedef enum Nextion_Baudrate_TypeDefinition{
	Baud_2400= 2400,
	Baud_4800 = 4800,
	Baud_9600 = 9600,
	Baud_19200 = 19200,
	Baud_38400 = 38400,
	Baud_57600 = 57600,
	Baud_115200 = 115200,
	Baud_230400 = 230400,
	Baud_256000 = 256000,
	Baud_512000 = 512000,
	Baud_921600 = 921600
}Nextion_Baudrate_TypeDef;

typedef enum Nextion_FontAlignment_TypeDefinition
{
  NEX_FA_LEFT_UP = 0,   //!< Left (x) or top (y)
  NEX_FA_CENTRE = 1,    //!< Centre
  NEX_FA_RIGHT_DOWN = 2 //!< Right (x) or bottom (y)
}Nextion_FontAlignment_TypeDef;

typedef struct Nextion_TouchEvent{
	uint8_t pageID;
	uint8_t componentID;
	uint8_t event;
}Nextion_Touch_TypedefStuct;

typedef struct Nextion_ReceiveFrame{
	uint8_t commandCode;
	uint8_t receiveData[10];
}Nextion_ReceiveFrame_TypedefStruct;

extern uint8_t nextionReceiveDataBuffer[256];
extern volatile bool nextionUartDataReceived;
//---------------------------------------------------------------------------------------------------------------
//Assign Statement
//---------------------------------------------------------------------------------------------------------------
//Write txt to specific control, clears previously write txt in control
void Nextion_WriteTxtToControlOnSpecWindow(const uint8_t *windowName, const uint8_t *controlName, const uint8_t *valueToWrite);
void Nextion_WriteTxtToControl(const uint8_t *controlName, const uint8_t *valueToWrite);
//Add txt to existing txt value in control
void Nextion_AddTxtToControlOnSpecWindow(const uint8_t *windowName, const uint8_t *controlName, const uint8_t *valueToWrite);
void Nextion_AddTxtToControl(const uint8_t *controlName, const uint8_t *valueToWrite);
//Delete selected number of chars from control
void Nextion_DeleteTxtToControlOnSpecWindow(const uint8_t *windowName, const uint8_t *controlName, const uint8_t numberOfCharsToDelete);
void Nextion_DeleteTxtToControl(const uint8_t *controlName, const uint8_t numberOfCharsToDelete);
//Change font size in control
void Nextion_ChangeFontSizeInControl(const uint8_t *windowName, const uint8_t *controlName, const uint8_t fontID);
//---------------------------------------------------------------------------------------------------------------
//Receive Msg
//---------------------------------------------------------------------------------------------------------------
Nextion_ReceiveFrame_TypedefStruct Nextion_DecodeReceiveMsg(uint8_t *recFrame, uint8_t recFrameFlag, uint8_t recMsgLength);
void Nextion_MakeActionForReceiveMsg(Nextion_ReceiveFrame_TypedefStruct *ReceiveMsgStructPtr, uint8_t msgLength);
//---------------------------------------------------------------------------------------------------------------
//Operational Commands
//---------------------------------------------------------------------------------------------------------------
//PAGE
void Nextion_LoadNewPage_PageId(const uint8_t pageId);
void Nextion_LoadNewPage_PagaName(const uint8_t *pageName, const uint8_t pageNameSize);
//REST
void Nextion_ResetDisplay(void);
//REF
void Nextion_RefreshDisplay_ComName(const uint8_t *componentName);
void Nection_RefreshDisplay_ComId(const uint8_t comId);
//CLICK
void Nextion_TriggerClick_ComName(const uint8_t *componentName, const uint8_t touchPressRelease);
void Nextion_TriggerClick_ComId(const uint8_t comId, const uint8_t touchPressRelease);
//REF_START
void Nextion_StopRefDefaultWaveform(void);
//REF_STOP
void Nextion_StopsRefDefaultWaveform(void);
//GET
void Nextion_SendAttribute(const uint8_t *attribute);
//SENDME
void Nextion_SendNumberOfCurrentlyLoadedPage(const uint8_t *attribute);
//---------------------------------------------------------------------------------------------------------------
//GUI Design Commands
//---------------------------------------------------------------------------------------------------------------
//CLS
void Nextion_ClearScreenWithColorConstant(const uint8_t *colorName, const uint8_t colorNameLength);
void Nextion_ClearScreenWithColorRGB565(const uint16_t colorRGB565);
//PIC
void Nextion_DispResoursePicture_Pic(const uint16_t xPos, const uint16_t yPos, const uint8_t picId);
//PICQ
void Nextion_CropPicture_Picq(const uint16_t xUpperLeftCorner, const uint16_t yUpperLeftCorner,
															const uint16_t widthCropArea, const uint16_t heightCropArea, const uint8_t picId);
//XPIC
void Nextion_AdvancedCropPicture_Xpic(const uint16_t xUpperLeftCorner, const uint16_t yUpperLeftCorner,
															const uint16_t widthCropArea, const uint16_t heightCropArea,
															const uint16_t xUpperLeftCornerCropArea, const uint16_t yUpperLeftCornerCropArea, const uint8_t picId);
//XSTR
void Nextion_WriteTxtOnScreen_XStr(const uint16_t xPos, const uint16_t yPos, const uint16_t contrWidth, const uint16_t contrHeight,
        							const uint8_t fontID, const uint32_t bgColour, const uint32_t fgColour, const uint8_t bgFill,
											const Nextion_FontAlignment_TypeDef xCentre, const Nextion_FontAlignment_TypeDef yCentre, const char *str);
//FILL
void Nextion_FillAreaWithColor_Fill(const uint16_t xStart, const uint16_t yStart, const uint16_t fillWidth, const uint16_t fillHeight, const uint16_t rgb565Colour);
//LINE
void Nextion_DrawLineOnScreen_Line(const uint16_t xStart, const uint16_t yStart, const uint16_t xEnd, const uint16_t yEnd, const uint16_t rgb565Colour);
//DRAW
void Nextion_DrawHollowRectange_Draw(const uint16_t xUpperLeftCorner, const uint16_t yUpperLeftCorner,
																			const uint16_t xLowerRightCorner, const uint16_t yLowerRightCorner, const uint16_t rgb565Colour);
//CIR
void Nextion_DrawHollowCircle_Cir(const uint16_t xCenterPoint, const uint16_t yCenterPoint, const uint16_t circleRadius, const uint16_t rgb565Colour);
//CIRS
void Nextion_DrawFilledCircle_Cirs(const uint16_t xCenterPoint, const uint16_t yCenterPoint, const uint16_t circleRadius, const uint16_t rgb565Colour);
//---------------------------------------------------------------------------------------------------------------
//System Variables
//---------------------------------------------------------------------------------------------------------------
//DIM
void Nextion_SetBacklight_Dim(uint8_t dimPercentValue);
//DIMS
void Nextion_SetBacklight_Dims(uint8_t dimPercentValue);
//BAUD
void Nextion_SetUartBaudrate_Baud(Nextion_Baudrate_TypeDef baudrateValue);
//BAUDS
void Nextion_SetUartBaudrate_Bauds(Nextion_Baudrate_TypeDef baudrateValue);
//SPAX
void Nextion_SetDefaultRenderingFontForXstr_Spax(const uint8_t fontValue);
//SPAY
void Nextion_SetDefaultRenderingSpaceForXstr_Spay(const uint16_t fontValue);
//THC
void Nextion_SetTouchDrawingBrushColor_Thc(const uint16_t color);
//THDRA
void Nextion_TurnsInternalDrawindFunct_Thdra(uint8_t functionValue);
//USSP
void Nextion_SetsInterNoSerSleepTim_Ussp(uint16_t usspTimer);
//THSP
void Nextion_SetsInterNoTouchSleepTim_Thsp(uint16_t thspTimerVal);
//THUP
void Nextion_AutoWakeOnTouch_Thup(uint16_t thupVal);
//SENDXY
void Nextion_RealTimeTouchCords_SendXy(uint16_t sendXySendingStatus);
//DELAY
void Nextion_DelayFuncMs_Delay(const uint16_t delayTime);
//SLEEP
void Nextion_EnterExitSleepMode_Sleep(uint8_t sleepModeVal);
//BKCMD
void Nextion_SetLevelReturnData_Bkcmd(uint8_t retDataLevel);
//WUP
void Nextion_WakeOnSerialData_Wup(const uint8_t wupValue);
//USUP
void Nextion_WakeOnSerialData_Usup(uint8_t usupValue);


#endif /* SRC_NEXTION_NEXTION_H_ */
