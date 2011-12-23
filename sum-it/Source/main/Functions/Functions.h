/*
	Copyright 1996, 1997, 1998, 2000
	        Hekkelman Programmatuur B.V.  All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.
	3. All advertising materials mentioning features or use of this software
	   must display the following acknowledgement:
	   
	    This product includes software developed by Hekkelman Programmatuur B.V.
	
	4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
	   promote products derived from this software without specific prior
	   written permission.
	
	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/
/*
	Functions.h
	
	Copyright 1997, Hekkelman Programmatuur
	
	Part of Sum-It for the BeBox version 1.1.

*/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

enum {
	kABSFuncNr,
	kACOSFuncNr,
	kANNUITYFuncNr,
	kASCFuncNr,
	kASINFuncNr,
	kATANFuncNr,
	kAVGFuncNr,
	kCEILINGFuncNr,
	kCELLFuncNr,
	kCHOOSEFuncNr,
	kCHRFuncNr,
	kCOLUMNFuncNr,
	kCOMPOUNDFuncNr,
	kCOSFuncNr,
	kCOTFuncNr,
	kCOUNTFuncNr,
	kDATEFuncNr,
	kDAYFuncNr,
	kDBFuncNr,
	kDOCUMENTFuncNr,
	kDOWFuncNr,
	kERRFuncNr,
	kERRORFuncNr,
	kEXPFuncNr,
	kFALSEFuncNr,
	kFLOORFuncNr,
	kFRACFuncNr,
	kFVFuncNr,
	kHINDEXFuncNr,
	kHLOOKUPFuncNr,
	kHOURFuncNr,
	kIFFuncNr,
	kIFERRFuncNr,
	kINTFuncNr,
	kIRRFuncNr,
	kISNULLFuncNr,
	kISNUMFuncNr,
	kISTEXTFuncNr,
	kLEFTFuncNr,
	kLENGTHFuncNr,
	kLNFuncNr,
	kLOGFuncNr,
	kMAXFuncNr,
	kMIDFuncNr,
	kMINFuncNr,
	kMINUTEFuncNr,
	kMODFuncNr,
	kMONTHFuncNr,
	kNAFuncNr,
	kNCOLSFuncNr,
	kNOWFuncNr,
	kNPVFuncNr,
	kNROWSFuncNr,
	kNUM2CFuncNr,
	kNUMPAGESFuncNr,
	kOFFSETFuncNr,
	kPAGEFuncNr,
	kPIFuncNr,
	kPMTFuncNr,
	kPVFuncNr,
	kRANDOMFuncNr,
	kRIGHTFuncNr,
	kROUNDFuncNr,
	kROWFuncNr,
	kSECONDFuncNr,
	kSIGNFuncNr,
	kSINFuncNr,
	kSLFuncNr,
	kSOYDFuncNr,
	kSQRTFuncNr,
	kSTDDEVFuncNr,
	kSUMFuncNr,
	kTANFuncNr,
	kTIMEFuncNr,
	kTIME2CFuncNr,
	kVARIANCEFuncNr,
	kVINDEXFuncNr,
	kVLOOKUPFuncNr,
	kYEARFuncNr,
	kTRUEFuncNr,
	kORFuncNr,
	kANDFuncNr,
	kPOWERFuncNr,
	kDEC2HEXFuncNr,
	kFunctionCount
};

void ABSFunction(Value *stack, int argCnt, CContainer *cells);
void ACOSFunction(Value *stack, int argCnt, CContainer *cells);
void ANNUITYFunction(Value *stack, int argCnt, CContainer *cells);
void ASCFunction(Value *stack, int argCnt, CContainer *cells);
void ASINFunction(Value *stack, int argCnt, CContainer *cells);
void ATANFunction(Value *stack, int argCnt, CContainer *cells);
void AVGFunction(Value *stack, int argCnt, CContainer *cells);
void CEILINGFunction(Value *stack, int argCnt, CContainer *cells);
void CELLFunction(Value *stack, int argCnt, CContainer *cells);
void CHOOSEFunction(Value *stack, int argCnt, CContainer *cells);
void CHRFunction(Value *stack, int argCnt, CContainer *cells);
void COLUMNFunction(Value *stack, int argCnt, CContainer *cells);
void COMPOUNDFunction(Value *stack, int argCnt, CContainer *cells);
void COSFunction(Value *stack, int argCnt, CContainer *cells);
void COTFunction(Value *stack, int argCnt, CContainer *cells);
void COUNTFunction(Value *stack, int argCnt, CContainer *cells);
void DATEFunction(Value *stack, int argCnt, CContainer *cells);
void DAYFunction(Value *stack, int argCnt, CContainer *cells);
void DBFunction(Value *stack, int argCnt, CContainer *cells);
void DOCUMENTFunction(Value *stack, int argCnt, CContainer *cells);
void DOWFunction(Value *stack, int argCnt, CContainer *cells);
void ERRFunction(Value *stack, int argCnt, CContainer *cells);
void ERRORFunction(Value *stack, int argCnt, CContainer *cells);
void EXPFunction(Value *stack, int argCnt, CContainer *cells);
void FALSEFunction(Value *stack, int argCnt, CContainer *cells);
void FLOORFunction(Value *stack, int argCnt, CContainer *cells);
void FRACFunction(Value *stack, int argCnt, CContainer *cells);
void FVFunction(Value *stack, int argCnt, CContainer *cells);
void HINDEXFunction(Value *stack, int argCnt, CContainer *cells);
void HLOOKUPFunction(Value *stack, int argCnt, CContainer *cells);
void HOURFunction(Value *stack, int argCnt, CContainer *cells);
void IFFunction(Value *stack, int argCnt, CContainer *cells);
void IFERRFunction(Value *stack, int argCnt, CContainer *cells);
void INTFunction(Value *stack, int argCnt, CContainer *cells);
void IRRFunction(Value *stack, int argCnt, CContainer *cells);
void ISNULLFunction(Value *stack, int argCnt, CContainer *cells);
void ISNUMFunction(Value *stack, int argCnt, CContainer *cells);
void ISTEXTFunction(Value *stack, int argCnt, CContainer *cells);
void LEFTFunction(Value *stack, int argCnt, CContainer *cells);
void LENGTHFunction(Value *stack, int argCnt, CContainer *cells);
void LNFunction(Value *stack, int argCnt, CContainer *cells);
void LOGFunction(Value *stack, int argCnt, CContainer *cells);
void MAXFunction(Value *stack, int argCnt, CContainer *cells);
void MIDFunction(Value *stack, int argCnt, CContainer *cells);
void MINFunction(Value *stack, int argCnt, CContainer *cells);
void MINUTEFunction(Value *stack, int argCnt, CContainer *cells);
void MODFunction(Value *stack, int argCnt, CContainer *cells);
void MONTHFunction(Value *stack, int argCnt, CContainer *cells);
void NAFunction(Value *stack, int argCnt, CContainer *cells);
void NCOLSFunction(Value *stack, int argCnt, CContainer *cells);
void NOWFunction(Value *stack, int argCnt, CContainer *cells);
void NPVFunction(Value *stack, int argCnt, CContainer *cells);
void NROWSFunction(Value *stack, int argCnt, CContainer *cells);
void NUM2CFunction(Value *stack, int argCnt, CContainer *cells);
void NUMPAGESFunction(Value *stack, int argCnt, CContainer *cells);
void OFFSETFunction(Value *stack, int argCnt, CContainer *cells);
void PAGEFunction(Value *stack, int argCnt, CContainer *cells);
void PIFunction(Value *stack, int argCnt, CContainer *cells);
void PMTFunction(Value *stack, int argCnt, CContainer *cells);
void PVFunction(Value *stack, int argCnt, CContainer *cells);
void RANDOMFunction(Value *stack, int argCnt, CContainer *cells);
void RIGHTFunction(Value *stack, int argCnt, CContainer *cells);
void ROUNDFunction(Value *stack, int argCnt, CContainer *cells);
void ROWFunction(Value *stack, int argCnt, CContainer *cells);
void SECONDFunction(Value *stack, int argCnt, CContainer *cells);
void SIGNFunction(Value *stack, int argCnt, CContainer *cells);
void SINFunction(Value *stack, int argCnt, CContainer *cells);
void SLFunction(Value *stack, int argCnt, CContainer *cells);
void SOYDFunction(Value *stack, int argCnt, CContainer *cells);
void SQRTFunction(Value *stack, int argCnt, CContainer *cells);
void STDDEVFunction(Value *stack, int argCnt, CContainer *cells);
void SUMFunction(Value *stack, int argCnt, CContainer *cells);
void TANFunction(Value *stack, int argCnt, CContainer *cells);
void TIMEFunction(Value *stack, int argCnt, CContainer *cells);
void TIME2CFunction(Value *stack, int argCnt, CContainer *cells);
void VARIANCEFunction(Value *stack, int argCnt, CContainer *cells);
void VINDEXFunction(Value *stack, int argCnt, CContainer *cells);
void VLOOKUPFunction(Value *stack, int argCnt, CContainer *cells);
void YEARFunction(Value *stack, int argCnt, CContainer *cells);
void TRUEFunction(Value *stack, int argCnt, CContainer *cells);
void ANDFunction(Value *stack, int argCnt, CContainer *cells);
void ORFunction(Value *stack, int argCnt, CContainer *cells);
void POWERFunction(Value *stack, int argCnt, CContainer *cells);
void DEC2HEXFunction(Value *stack, int argCnt, CContainer *cells);

#endif
