
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "nextion.h"

/*
 * Commands LIST:
 * 3-OPERATIONAL COMMANDS
 * + page
 * + ref
 * + click
 * + ref_stop
 * + ref_start
 * + get
 * - sendme
 * - covx
 * - cov
 * - touch_j
 * - substr
 * - vis
 * - tsw
 * - com_stop
 * - com_star
 * - randset
 * - code_c
 * - prints
 * - print
 * - printh
 * - add
 * - addt
 * - cle
 * + rest
 * - doevents
 * - strlen
 * - btlen
 * 4-GUI Designing commandSize
 * + cls
 * + pic
 * + picq
 * + xpic
 * + xstr
 * + fill
 * + line
 * + draw
 * + cir
 * + cirs
 * 6-System Variables
 * + dim
 * + dims
 * + baud
 * + bauds
 * + spax
 * + spay
 * + thc
 * + thdra
 * + ussp
 * + thsp
 * + thup
 * + sendxy
 * + delay
 * + sleep
 * + bkcmd
 * - rand
 * - sys0
 * - sys1
 * - sys2
 * + wup
 * + usup
 * - addr
 * - tch0
 * - tch1
 * - tch2
 * - tch3
 * - recmod
 * - usize


 * */

static void USART_SendChar(const uint8_t c);
static size_t USART_SendString(const uint8_t * str);

static size_t Nextion_SendCommand(uint8_t *command, uint8_t commandSize);

uint8_t nextionReceiveDataBuffer[256];
volatile bool nextionUartDataReceived;
//---------------------------------------------------------------------------------------------
//Assignment Statements
//---------------------------------------------------------------------------------------------
/*
 * Modify txt value in control:
 * ex. Nextion_WriteTxtToControlOnSpecWindow((const uint8_t *)"main", (const uint8_t *)"t0", (const uint8_t *)"20:12");
 */
void Nextion_WriteTxtToControlOnSpecWindow(const uint8_t *windowName, const uint8_t *controlName, const uint8_t *valueToWrite) {
	uint8_t dataToWrite[100] = {0x00};
	uint8_t size = sprintf((char *)dataToWrite, "%s.%s.txt=\"%s\"", windowName, controlName, valueToWrite);

	Nextion_SendCommand(dataToWrite, size + 1);
}

/*
 * Modify txt value in control:
 * ex. Nextion_WriteTxtToControl((const uint8_t *)"t0", (const uint8_t *)"20:12");
 */
void Nextion_WriteTxtToControl(const uint8_t *controlName, const uint8_t *valueToWrite) {
	uint8_t dataToWrite[100] = {0x00};
	uint8_t size = sprintf((char *)dataToWrite, "%s.txt=\"%s\"", controlName, valueToWrite);

	Nextion_SendCommand(dataToWrite, size + 1);
}
/*
 * Add txt value in control:
 * ex. Nextion_AddTxtToControlOnSpecWindow((const uint8_t *)"main", (const uint8_t *)"t0", (const uint8_t *)"20:12");
 */
void Nextion_AddTxtToControlOnSpecWindow(const uint8_t *windowName, const uint8_t *controlName, const uint8_t *valueToWrite) {
	uint8_t dataToWrite[100] = {0x00};
	uint8_t size = sprintf((char *)dataToWrite, "%s.%s.txt+=\"%s\"", windowName, controlName, valueToWrite);

	Nextion_SendCommand(dataToWrite, size + 1);
}
/*
 * Add txt value in control:
 * ex. Nextion_AddTxtToControl((const uint8_t *)"t0", (const uint8_t *)"20:12");
 */
void Nextion_AddTxtToControl(const uint8_t *controlName, const uint8_t *valueToWrite) {
	uint8_t dataToWrite[100] = {0x00};
	uint8_t size = sprintf((char *)dataToWrite, "%s.txt+=\"%s\"", controlName, valueToWrite);

	Nextion_SendCommand(dataToWrite, size + 1);
}

/*
 * Delete single txt value in control:
 * ex. Nextion_DeleteTxtToControlOnSpecWindow((const uint8_t *)"main", (const uint8_t *)"t0", 2);
 */
void Nextion_DeleteTxtToControlOnSpecWindow(const uint8_t *windowName, const uint8_t *controlName, const uint8_t numberOfCharsToDelete) {
	uint8_t dataToWrite[100] = {0x00};
	uint8_t size = sprintf((char *)dataToWrite, "%s.%s.txt-=%u", windowName, controlName, numberOfCharsToDelete);

	Nextion_SendCommand(dataToWrite, size + 1);
}
/*
 * Delete single txt value in control:
 * ex. Nextion_DeleteTxtToControl((const uint8_t *)"t0", 2);
 */
void Nextion_DeleteTxtToControl(const uint8_t *controlName, const uint8_t numberOfCharsToDelete) {
	uint8_t dataToWrite[100] = {0x00};
	uint8_t size = sprintf((char *)dataToWrite, "%s.txt-=%u", controlName, numberOfCharsToDelete);

	Nextion_SendCommand(dataToWrite, size + 1);
}

void Nextion_ChangeFontSizeInControl(const uint8_t *windowName, const uint8_t *controlName, const uint8_t fontID)
{
	uint8_t dataToWrite[100] = {0x00};
	uint8_t size = sprintf((char *)dataToWrite, "%s.%s.font=%u", windowName, controlName, fontID);

	Nextion_SendCommand(dataToWrite, size + 1);
}
//---------------------------------------------------------------------------------------------
//Operational Commands
//---------------------------------------------------------------------------------------------
/*
 * PAGE
 *	ex. Nextion_LoadNewPage_PageId(1);
 *	ex. Nextion_LoadNewPage_PagaName("main", 4);
*/
void Nextion_LoadNewPage_PageId(const uint8_t pageId)
{
	uint8_t pageCommand[6] = {0x00};

	pageCommand[0] = 'p';
	pageCommand[1] = 'a';
	pageCommand[2] = 'g';
	pageCommand[3] = 'e';
	pageCommand[4] = ' ';
	pageCommand[5] = pageId + 0x30;

	Nextion_SendCommand(pageCommand, sizeof(pageCommand) + 1);
}
void Nextion_LoadNewPage_PagaName(const uint8_t *pageName, const uint8_t pageNameSize)
{
	uint8_t pageCommand[30] = {0x00};

	pageCommand[0] = 'p';
	pageCommand[1] = 'a';
	pageCommand[2] = 'g';
	pageCommand[3] = 'e';
	pageCommand[4] = ' ';

	for(uint8_t i=0; i<pageNameSize; i++)
	{
		pageCommand[i + 5] = *(pageName + i);
	}

	Nextion_SendCommand(pageCommand, (6 + pageNameSize));
}
//REFRESH
void Nextion_RefreshDisplay_ComName(const uint8_t *componentName)
{
	uint8_t commandBuffer[20] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "ref %s", componentName);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}

//ref 0 -> Refresh all components
void Nection_RefreshDisplay_ComId(const uint8_t comId)
{
	uint8_t commandBuffer[20] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "ref %u", comId);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}

//CLICK
// @param: touchPressRelease - 1 Touch Press Event
//							   0 Touch Release Event
void Nextion_TriggerClick_ComName(const uint8_t *componentName, const uint8_t touchPressRelease)
{
	uint8_t commandBuffer[20] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "ref %s,%u", componentName, touchPressRelease);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}

void Nextion_TriggerClick_ComId(const uint8_t comId, const uint8_t touchPressRelease)
{
	uint8_t commandBuffer[20] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "ref %u,%u", comId, touchPressRelease);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}
//REF_STOP
void Nextion_StopRefDefaultWaveform(void)
{
	uint8_t commandBuffer[] = "ref_stop\0";
	Nextion_SendCommand(commandBuffer, sizeof(commandBuffer));
}

//REF_START
void Nextion_StartRefDefaultWaveform(void)
{
	uint8_t commandBuffer[] = "ref_start\0";
	Nextion_SendCommand(commandBuffer, sizeof(commandBuffer));
}
//GET
void Nextion_SendAttribute(const uint8_t *attribute)
{
	uint8_t commandBuffer[40] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "get %s", attribute);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}
//SENDME
void Nextion_SendNumberOfCurrentlyLoadedPage(const uint8_t *attribute)
{
	uint8_t commandBuffer[] = "sendme\0";
	Nextion_SendCommand(commandBuffer, sizeof(commandBuffer));
}
//COVX

//COV
//TOUCH_J
//SUBSTR
//VIS
//TSW
//COM_STOP
//COM_START
//RANDSET
//CODE_C
//PRINTS
//PRINT
//PRINTH
//ADD
//ADDT
//CLE
//RESET
void Nextion_ResetDisplay(void)
{
	uint8_t resetCommand[] = "rest";
	Nextion_SendCommand(resetCommand, sizeof(resetCommand));
}
//DOEVENTS
//STRLEN
//BTLEN
//---------------------------------------------------------------------------------------------
//GUI designing commands
//---------------------------------------------------------------------------------------------
/* CLS
 *  ex. Nextion_ClearScreenWithColorRGB565(48192);
 *  ex. Nextion_ClearScreenWithColorConstant("BLUE", 4);
 * */
void Nextion_ClearScreenWithColorConstant(const uint8_t *colorName, const uint8_t colorNameLength)
{
	uint8_t clearScreenColorFrame[30] = {0x00};

	clearScreenColorFrame[0] = 'c';
	clearScreenColorFrame[1] = 'l';
	clearScreenColorFrame[2] = 's';
	clearScreenColorFrame[3] = ' ';

	for(uint8_t i=0; i<colorNameLength; i++)
	{
		clearScreenColorFrame[i + 4] = *(colorName + i);
	}

	Nextion_SendCommand(clearScreenColorFrame, (5 + colorNameLength));
}
void Nextion_ClearScreenWithColorRGB565(const uint16_t colorRGB565)
{
	uint8_t clearScreenColorFrame[30] = {0x00};
	uint8_t frameSize = sprintf((char *)clearScreenColorFrame, "cls %u", colorRGB565);

	Nextion_SendCommand(clearScreenColorFrame, (frameSize + 1));
}
//PIC
void Nextion_DispResoursePicture_Pic(const uint16_t xPos, const uint16_t yPos, const uint8_t picId)
{
  uint8_t commandBuffer[30] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "pic %u,%u,%u", xPos, yPos, picId);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}
//PICQ
void Nextion_CropPicture_Picq(const uint16_t xUpperLeftCorner, const uint16_t yUpperLeftCorner,
															const uint16_t widthCropArea, const uint16_t heightCropArea, const uint8_t picId)
{
	uint8_t commandBuffer[30] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "picq %u,%u,%u,%u,%u", xUpperLeftCorner, yUpperLeftCorner, widthCropArea, heightCropArea, picId);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}
//XPIC
void Nextion_AdvancedCropPicture_Xpic(const uint16_t xUpperLeftCorner, const uint16_t yUpperLeftCorner,
															const uint16_t widthCropArea, const uint16_t heightCropArea,
															const uint16_t xUpperLeftCornerCropArea, const uint16_t yUpperLeftCornerCropArea, const uint8_t picId)
{
	uint8_t commandBuffer[30] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "picq %u,%u,%u,%u,%u,%u,%u", xUpperLeftCorner, yUpperLeftCorner, widthCropArea, heightCropArea,
	 																																					xUpperLeftCornerCropArea, yUpperLeftCornerCropArea, picId);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}
//XSTR
/*
 * @bgType - is background Fill (0 – crop image, 1 – solid color, 2 – image, 3 – none)
 */
void Nextion_WriteTxtOnScreen_XStr(const uint16_t xPos, const uint16_t yPos, const uint16_t contrWidth, const uint16_t contrHeight,
        							const uint8_t fontID, const uint32_t bgColour, const uint32_t fgColour, const uint8_t bgFill,
											const Nextion_FontAlignment_TypeDef xCentre, const Nextion_FontAlignment_TypeDef yCentre, const char *str)
{
	uint8_t commandBuffer[200] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "xstr %u,%u,%u,%u,%u,%lu,%lu,%u,%u,%u,\"%s\"",
			xPos, yPos, contrWidth, contrHeight, fontID, fgColour, bgColour, xCentre, yCentre, bgFill, str);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}
//FILL
void Nextion_FillAreaWithColor_Fill(const uint16_t xStart, const uint16_t yStart, const uint16_t fillWidth, const uint16_t fillHeight, const uint16_t rgb565Colour)
{
	uint8_t commandBuffer[30] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "fill %u,%u,%u,%u,%u",
			xStart, yStart, fillWidth, fillHeight, rgb565Colour);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}
//LINE
void Nextion_DrawLineOnScreen_Line(const uint16_t xStart, const uint16_t yStart, const uint16_t xEnd, const uint16_t yEnd, const uint16_t rgb565Colour)
{
	uint8_t commandBuffer[30] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "line %u,%u,%u,%u,%u",
			xStart, yStart, xEnd, yEnd, rgb565Colour);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}
//DRAW
void Nextion_DrawHollowRectange_Draw(const uint16_t xUpperLeftCorner, const uint16_t yUpperLeftCorner,
																			const uint16_t xLowerRightCorner, const uint16_t yLowerRightCorner, const uint16_t rgb565Colour)
{
	uint8_t commandBuffer[30] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "draw %u,%u,%u,%u,%u",
			xUpperLeftCorner, yUpperLeftCorner, xLowerRightCorner, yLowerRightCorner, rgb565Colour);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}
//CIR
void Nextion_DrawHollowCircle_Cir(const uint16_t xCenterPoint, const uint16_t yCenterPoint, const uint16_t circleRadius, const uint16_t rgb565Colour)
{
	uint8_t commandBuffer[30] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "cir %u,%u,%u,%u",
			xCenterPoint, yCenterPoint, circleRadius, rgb565Colour);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}
//CIRS
void Nextion_DrawFilledCircle_Cirs(const uint16_t xCenterPoint, const uint16_t yCenterPoint, const uint16_t circleRadius, const uint16_t rgb565Colour)
{
	uint8_t commandBuffer[30] = {0x00};
	uint8_t frameSize = sprintf((char *)commandBuffer, "cirs %u,%u,%u,%u",
			xCenterPoint, yCenterPoint, circleRadius, rgb565Colour);
	Nextion_SendCommand(commandBuffer, (frameSize + 1));
}
//---------------------------------------------------------------------------------------------
//System Variables
//---------------------------------------------------------------------------------------------
//DIM
void Nextion_SetBacklight_Dim(uint8_t dimPercentValue)
{
	if(dimPercentValue > 100) {
		dimPercentValue = 100;
	}

	uint8_t dimFrame[30] = {0x00};
	uint8_t frameSize = sprintf((char *)dimFrame, "dim=%u", dimPercentValue);

	Nextion_SendCommand(dimFrame, (frameSize + 1));
}
//DIMS
void Nextion_SetBacklight_Dims(uint8_t dimPercentValue)
{
	if(dimPercentValue > 100) {
		dimPercentValue = 100;
	}

	uint8_t dimsFrame[30] = {0x00};
	uint8_t frameSize = sprintf((char *)dimsFrame, "dims=%u", dimPercentValue);

	Nextion_SendCommand(dimsFrame, (frameSize + 1));
}
//BAUD
void Nextion_SetUartBaudrate_Baud(Nextion_Baudrate_TypeDef baudrateValue)
{
	uint8_t baudFrame[30] = {0x00};
	uint8_t frameSize = sprintf((char *)baudFrame, "baud=%u", baudrateValue);

	Nextion_SendCommand(baudFrame, (frameSize + 1));
}
//BAUDS
void Nextion_SetUartBaudrate_Bauds(Nextion_Baudrate_TypeDef baudrateValue)
{
	uint8_t baudsFrame[30] = {0x00};
	uint8_t frameSize = sprintf((char *)baudsFrame, "bauds=%u", baudrateValue);

	Nextion_SendCommand(baudsFrame, (frameSize + 1));
}
//SPAX
void Nextion_SetDefaultRenderingFontForXstr_Spax(const uint8_t spaxValue)
{
  uint8_t spaxFrame[20] = {0x00};
  uint8_t frameSize = sprintf((char *)spaxFrame, "spax=%u", spaxValue);

  Nextion_SendCommand(spaxFrame, (frameSize + 1));
}

//SPAY
void Nextion_SetDefaultRenderingSpaceForXstr_Spay(const uint16_t spyValue)
{
  uint8_t spayFrame[20] = {0x00};
  uint8_t frameSize = sprintf((char *)spayFrame, "spay=%u", spyValue);

  Nextion_SendCommand(spayFrame, (frameSize + 1));
}

//THC
void Nextion_SetTouchDrawingBrushColor_Thc(const uint16_t color)
{
  uint8_t tchFrame[20] = {0x00};
  uint8_t frameSize = sprintf((char *)tchFrame, "thc=%u", color);

  Nextion_SendCommand(tchFrame, (frameSize + 1));
}

//THDRA
void Nextion_TurnsInternalDrawindFunct_Thdra(uint8_t functionValue)
{
  if(functionValue != 0 && functionValue != 1)
  {
    functionValue = 1;
  }

  uint8_t thdraFrame[8] = {0x00};
  thdraFrame[0] = 't';
  thdraFrame[1] = 'h';
  thdraFrame[2] = 'd';
  thdraFrame[3] = 'r';
  thdraFrame[4] = 'a';
  thdraFrame[5] = '=';
  thdraFrame[6] = functionValue + 0x30;

  Nextion_SendCommand(thdraFrame, sizeof(thdraFrame));
}
//USSP
void Nextion_SetsInterNoSerSleepTim_Ussp(uint16_t usspTimer)
{
	if(usspTimer < 3)
	{
		usspTimer = 3;
	}

	uint8_t usspFrame[20] = {0x00};
    uint8_t frameSize = sprintf((char *)usspFrame, "ussp=%u", usspTimer);

    Nextion_SendCommand(usspFrame, (frameSize + 1));
}
//THSP
void Nextion_SetsInterNoTouchSleepTim_Thsp(uint16_t thspTimerVal)
{
	if(thspTimerVal < 3)
	{
		thspTimerVal = 3;
	}
	uint8_t thspFrame[20] = {0x00};
	uint8_t frameSize = sprintf((char *)thspFrame, "thsp=%u", thspTimerVal);

	Nextion_SendCommand(thspFrame, (frameSize + 1));
}
//THUP
void Nextion_AutoWakeOnTouch_Thup(uint16_t thupVal)
{
	if(thupVal != 0 && thupVal != 1)
	{
		thupVal = 0;
	}

	uint8_t thupFrame[7] = {0x00};
	thupFrame[0] = 't';
	thupFrame[1] = 'h';
	thupFrame[2] = 'u';
	thupFrame[3] = 'p';
	thupFrame[4] = '=';
	thupFrame[5] = thupVal + 0x30;

	Nextion_SendCommand(thupFrame, sizeof(thupFrame));
}
//SENDXY
void Nextion_RealTimeTouchCords_SendXy(uint16_t sendXySendingStatus)
{
	if(sendXySendingStatus != 0 && sendXySendingStatus != 1)
	{
		sendXySendingStatus = 0;
	}

	uint8_t sendXyFrame[9] = {0x00};
	sendXyFrame[0] = 's';
	sendXyFrame[1] = 'e';
	sendXyFrame[2] = 'n';
	sendXyFrame[3] = 'd';
	sendXyFrame[4] = 'x';
	sendXyFrame[5] = 'y';
	sendXyFrame[6] = '=';
	sendXyFrame[7] = sendXySendingStatus + 0x30;

	Nextion_SendCommand(sendXyFrame, sizeof(sendXyFrame));
}
//DELAY
void Nextion_DelayFuncMs_Delay(const uint16_t delayTime)
{
  uint8_t delayFrame[20] = {0x00};
  uint8_t frameSize = sprintf((char *)delayFrame, "delay=%u", delayTime);

  Nextion_SendCommand(delayFrame, (frameSize + 1));
}
//SLEEP
//@param: sleepModeVal - 1 enter sleep sleep mode
//						0 exit sleep mode
//                      default 0
void Nextion_EnterExitSleepMode_Sleep(uint8_t sleepModeVal)
{
	if(sleepModeVal != 0 && sleepModeVal != 1)
	{
		sleepModeVal = 0;
	}

	uint8_t sleepFrame[8] = {0x00};
	sleepFrame[0] = 's';
	sleepFrame[1] = 'l';
	sleepFrame[2] = 'e';
	sleepFrame[3] = 'e';
	sleepFrame[4] = 'p';
	sleepFrame[5] = '=';
	sleepFrame[6] = sleepModeVal + 0x30;

	Nextion_SendCommand(sleepFrame, sizeof(sleepFrame));
}
//BKCMD
//@param: retDataLevel - 0 - 3 default 2
//– Level 0 is Off – no pass/fail will be returned
//– Level 1 is OnSuccess, only when last serial command successful.
//– Level 2 is OnFailure, only when last serial command failed
//– Level 3 is Always, returns 0x00 to 0x23 result of serial command.
void Nextion_SetLevelReturnData_Bkcmd(uint8_t retDataLevel)
{
	if(retDataLevel > 3)
	{
		retDataLevel = 2;
	}

	uint8_t bkcmdFrame[8] = {0x00};
	bkcmdFrame[0] = 'b';
	bkcmdFrame[1] = 'k';
	bkcmdFrame[2] = 'c';
	bkcmdFrame[3] = 'm';
	bkcmdFrame[4] = 'd';
	bkcmdFrame[5] = '=';
	bkcmdFrame[6] = retDataLevel + 0x30;

	Nextion_SendCommand(bkcmdFrame, sizeof(bkcmdFrame));
}
//WUP
void Nextion_WakeOnSerialData_Wup(const uint8_t wupValue)
{
	uint8_t wupFrame[20] = {0x00};
	uint8_t frameSize = sprintf((char *)wupFrame, "wup=%u", wupValue);

	Nextion_SendCommand(wupFrame, (frameSize + 1));
}
//USUP
void Nextion_WakeOnSerialData_Usup(uint8_t usupValue)
{
	if(usupValue != 0 && usupValue != 1)
	{
		usupValue = 0;
	}

	uint8_t usupFrame[7] = {0x00};
	usupFrame[0] = 'u';
	usupFrame[1] = 's';
	usupFrame[2] = 'u';
	usupFrame[3] = 'p';
	usupFrame[4] = '=';
	usupFrame[5] = usupValue + 0x30;

	Nextion_SendCommand(usupFrame, sizeof(usupFrame));
}
//---------------------------------------------------------------------------------------------
//CHECK RECEIVE MSG
//---------------------------------------------------------------------------------------------
Nextion_ReceiveFrame_TypedefStruct Nextion_DecodeReceiveMsg(uint8_t *recFrame, uint8_t recFrameFlag, uint8_t recMsgLength)
{
    Nextion_ReceiveFrame_TypedefStruct Nextion_ReceiveFrame;
    Nextion_ReceiveFrame.commandCode = 0x00;

    for(uint8_t i=0; i<sizeof(Nextion_ReceiveFrame.receiveData); i++)
    {
        Nextion_ReceiveFrame.receiveData[i] = 0x00;
    }

    if(recFrameFlag == 0)
    {
        Nextion_ReceiveFrame.commandCode = 0x31;
        return Nextion_ReceiveFrame; //No Frame Received
    }

    if(recMsgLength < 4 || recMsgLength > 9)
    {
        Nextion_ReceiveFrame.commandCode = 0x32;
        return Nextion_ReceiveFrame; //Receive too few or too many bytes
    }

    Nextion_ReceiveFrame.commandCode = *(recFrame + 0);

    if(recMsgLength == 4)
    {
        return Nextion_ReceiveFrame;
    }

    for(uint8_t i=0; i<(recMsgLength - 4); i++)
    {
        Nextion_ReceiveFrame.receiveData[i] = *(recFrame + i + 1);
    }

    return Nextion_ReceiveFrame;
}

void Nextion_MakeActionForReceiveMsg(Nextion_ReceiveFrame_TypedefStruct *ReceiveMsgStructPtr, uint8_t msgLength)
{
	switch(ReceiveMsgStructPtr->commandCode)
	{
		case NEXTION_INVALID_INSTRUCTION_CODE: //NEXTION_START_UP_CODE
			if(msgLength == 6)
			{
				//NEXTION_START_UP_CODE
			}
			else
			{
				//NEXTION_INVALID_INSTRUCTION_CODE
			}
			break;
		case NEXTION_INSTRUCTION_SUCCESSFUL_CODE:
			/* */
			break;
		case NEXTION_INVALID_COMPONENT_ID_CODE:
			/* */
			break;
		case NEXTION_INVALID_PAGE_ID_CODE:
			/* */
			break;
		case NEXTION_INVALID_PICTURE_ID_CODE:
			/* */
			break;
		case NEXTION_INVALID_FONT_ID:
			/* */
			break;
		case NEXTION_INVALID_FILE_OPERAT_CODE:
			/* */
			break;
		case NEXTION_INVALID_CRC_CODE:
			/* */
			break;
		case NEXTION_INVALID_BAUDRATE_CODE:
			/* */
			break;
		case NEXTION_INVALID_WAVEFORM_ID_CHANNEL_CODE:
			/* */
			break;
		case NEXTION_INVALID_VAR_NAME_ATRRIBUTE_CODE:
			/* */
			break;
		case NEXTION_INVALID_VAR_OPERATION_CODE:
			/* */
			break;
		case NEXTION_FAILED_ASSIGNMENT_CODE:
			/* */
			break;
		case NEXTION_FAILED_EEPROM_OPER_CODE:
			/* */
			break;
		case NEXTION_INVALID_QUANT_OF_PARAM_CODE:
			/* */
			break;
		case NEXTION_FAILED_IO_OPERAT_CODE:
			/* */
			break;
		case NEXTION_INVALID_ESCAPE_CHAR_CODE:
			/* */
			break;
		case NEXTION_TOO_LONG_VAR_NAME_CODE:
			/* */
			break;
		case NEXTION_SERIAL_BUFFER_OVERFLOW:
			/* */
			break;
		case NEXTION_RETURN_CODE_TOUCH_EVENT:
			/* */
			break;
		case NEXTION_CURRENT_PAGE_NUMBER:
			/* */
			break;
		case NEXTION_TOUCH_COORDINATE_AWAKE:
			/* */
			break;
		case NEXTION_TOUCH_COORDINATE_SLEEP:
			/* */
			break;
		case NEXTION_STRING_DATA_ENCLOSED:
			/* */
			break;
		case NEXTION_NUMERIC_DATA_ENCLOSED:
			/* */
			break;
		case NEXTION_AUTO_ENTERED_SLEEP_MODE:
			/* */
			break;
		case NEXTION_AUTO_WAKE_FROM_SLEEP:
			/* */
			break;
		case NEXTION_READY:
			/* */
			break;
		case NEXTION_START_MICRO_SD_UPGRADE:
			/* */
			break;
		case NEXTION_TRANSPARENT_DATA_FINISHED:
			/* */
			break;
		case NEXTION_TRANSPARENT_DATA_READY:
			/* */
			break;
		default:
		break;
	}
}
//---------------------------------------------------------------------------------------------
/*
 * 	@brief	Function use to send frame prepared for Nextion displays. It add 3x0xFF to passed command
 * 	@param	command: pointer to command that need to be send to device
 * 	@param  commandSize: size of command to send
 * 	@ret	number of send data
 */
static size_t Nextion_SendCommand(uint8_t *command, const uint8_t commandSize)
{
	uint8_t arrayToSend[(commandSize - 1) + 4];

	for(uint8_t i = 0; i<sizeof(arrayToSend); i++)
	{
		arrayToSend[i] = 0x00;
	}

	for(uint8_t loop = 0; loop < (commandSize - 1); loop++) {
		arrayToSend[loop] = *(command + loop);
	}

	arrayToSend[commandSize - 1] = 0xFF;
	arrayToSend[commandSize + 0] = 0xFF;
	arrayToSend[commandSize + 1] = 0xFF;
	arrayToSend[commandSize + 2] = 0x00;

	return USART_SendString(&arrayToSend[0]);
}
//---------------------------------------------------------------------------------------------
//Receive data
//---------------------------------------------------------------------------------------------
#ifdef LPC1769
static uint8_t UART_RecData(uint8_t * rxbuf, uint32_t buflen) {

	uint32_t bToRecv, bRecv, timeOut;
	uint8_t *pChar = rxbuf;

	bToRecv = buflen;
	bRecv = 0;

	while (bToRecv) {
		timeOut = 0x00000FFFFUL;//UART_BLOCKING_TIMEOUT;
		while (!(LPC_UART0->LSR & UART_LSR_RDR)){
			if (timeOut == 0) break;
			timeOut--;
		}
		// Time out!
		if(timeOut == 0) break;
		// Get data from the buffer
		uint8_t rcvByte = UART_ReceiveData((LPC_UART_TypeDef *)LPC_UART0);
		(*pChar++) = rcvByte;
		bToRecv--;
		bRecv++;
	}
	return bRecv;
}

void UART0_IRQHandler(void) {
	uint32_t iirSource = 0;

	iirSource = LPC_UART0->IIR;
	iirSource &= UART_IIR_INTID_MASK;

	if(iirSource == UART_IIR_INTID_RDA) {
		memset(nextionReceiveDataBuffer, 0, sizeof nextionReceiveDataBuffer);
		if(UART_RecData(nextionReceiveDataBuffer, sizeof nextionReceiveDataBuffer))
		{
			nextionUartDataReceived = true;
			NVIC_DisableIRQ(UART0_IRQn);
		}
	}
}
/*
 * 	@brief	Send a single character via UART interface.
 * 	@param	c: character to be sent
 * 	@ret	none
 */
static void USART_SendChar(const uint8_t c)
{
	while( !(LPC_UART0->LSR & UART_LSR_THRE) );

	LPC_UART0->THR = c;
}

/*
 * 	@brief	Send a string via UART interface.
 * 	@param	str: a pointer to a character array
 * 	@ret	size_t: total numbers of characters sent
 */
static size_t USART_SendString(const uint8_t * str) {
	uint16_t charCount = 0;

	while(*str) {
		USART_SendChar(*str++);
		charCount++;
	}
	return charCount;
}
#elif defined STM32
#endif
