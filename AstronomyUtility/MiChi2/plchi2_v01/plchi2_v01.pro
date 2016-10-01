; 
; This code is for 
; plotting the SEDs with best-fit chi^2
; 
; The input files should have the same format for 
; michi2 code
; The first input is flux.dat file, 
; the second input is a list of lib files, 
; and the third input is the michi2 output chi2 file. 
; 
; The output of this code will be
; One EPS SED plot. 
; 
; 
PRO PlChi2_v01, InputDAT, InputLIB, InputFIT, OutputName=OutputName, SET_FONT=SET_FONT, SET_XRANGE=SET_XRANGE, SET_YRANGE=SET_YRANGE, $
                                              Source=Source, Redshift=Redshift, Distance=Distance
    
    ; 
    
    ; resolve_all
    CD, C=CurrentPath
    IF NOT STRMATCH(!PATH,CurrentPath+"/idlcrab/crabarray:"+CurrentPath+"/idlcrab/crabstring:"+CurrentPath+"/idlcrab/crabtable:") THEN BEGIN
        !PATH = CurrentPath+"/idlcrab/crabarray:"+CurrentPath+"/idlcrab/crabstring:"+CurrentPath+"/idlcrab/crabtable:" + !PATH
    ENDIF
    resolve_all
    
    ; Welcome
    PRINT, "Welcome!"
    
    ; Check Command Line Argument
    InputCommandLineArguments = COMMAND_LINE_ARGS(Count=InputCommandLineArgCount)
    IF InputCommandLineArgCount GE 1 THEN BEGIN
        InputDAT = []
        InputLIB = []
        InputFIT = []
        PRINT, "Command line arguments: "
        FOR i = 0, InputCommandLineArgCount-1 DO BEGIN
            ;PRINT, InputCommandLineArguments[i]
            IF i EQ 0 THEN BEGIN
                InputDAT = InputCommandLineArguments[i]
                PRINT, "InputDAT = "+InputCommandLineArguments[i]
            ENDIF ELSE IF i NE InputCommandLineArgCount-1 THEN BEGIN
                InputLIB = [ InputLIB, InputCommandLineArguments[i] ]
                PRINT, "InputLIB += "+InputCommandLineArguments[i]
            ENDIF ELSE IF i EQ InputCommandLineArgCount-1 THEN BEGIN
                InputFIT = InputCommandLineArguments[i]
                PRINT, "InputFIT = "+InputCommandLineArguments[i]
            ENDIF
        ENDFOR
    ENDIF
    
    ; Check InputDAT
    IF N_ELEMENTS(InputDAT) EQ 0 THEN MESSAGE, "Error! InputDAT is not given!", /CONTINUE
    
    ; Check InputLIB
    IF N_ELEMENTS(InputLIB) EQ 0 THEN MESSAGE, "Error! InputLIB is not given!", /CONTINUE
    
    ; Check InputFIT
    IF N_ELEMENTS(InputFIT) EQ 0 THEN MESSAGE, "Error! InputFIT is not given!", /CONTINUE
    
    ; Check All
    IF N_ELEMENTS(InputDAT) EQ 0 OR $
       N_ELEMENTS(InputLIB) EQ 0 OR $
       N_ELEMENTS(InputFIT) EQ 0 THEN BEGIN
        PRINT, ""
        PRINT, "Usage: "
        PRINT, "    PlChi2_v01, InputDAT, InputLIB, InputFIT"
        PRINT, ""
        PRINT, "Example: "
        PRINT, "    PlChi2_v01, 'flux_obsframe.dat', ['FSPS.Padova.BaSeL.Z0.0190.EBV.lib.SED','MullaneyAGN.lib.SED','DL07.HiExCom.lib.SED','DL07.LoExCom.lib.SED'], 'fit_quadruple.out'"
        PRINT, ""
        PRINT, "Alternative:"
        PRINT, "    We can run from bash command line"
        PRINT, "    idl -e PlChi2_v01 -args flux_obsframe.dat FSPS.Padova.BaSeL.Z0.0190.EBV.lib.SED MullaneyAGN.lib.SED DL07.HiExCom.lib.SED DL07.LoExCom.lib.SED fit_quadruple.out"
        PRINT, ""
        RETURN
    ENDIF
    
    ; Reform InputDAT
    IF N_ELEMENTS(InputDAT) EQ 1 THEN PlChi2_DAT = InputDAT ELSE PlChi2_DAT = InputDAT[0]
    
    ; Reform InputLIB
    IF N_ELEMENTS(InputLIB) EQ 1 THEN PlChi2_LIB = [InputLIB] ELSE PlChi2_LIB = InputLIB
    
    ; Reform InputFIT
    IF N_ELEMENTS(InputFIT) EQ 1 THEN PlChi2_FIT = InputFIT ELSE PlChi2_FIT = InputFIT[0]
    
    ; Check file existennce
    Check_OK = 1
    IF FILE_TEST(PlChi2_DAT) EQ 0 THEN BEGIN
        MESSAGE, "Error! "+PlChi2_DAT+" is not found!", /CONTINUE
        Check_OK = 0
    ENDIF
    FOREACH Temp_LIB, PlChi2_LIB DO BEGIN 
        IF FILE_TEST(Temp_LIB) EQ 0 THEN BEGIN
            MESSAGE, "Error! "+Temp_LIB+" is not found!", /CONTINUE
            Check_OK = 0
        ENDIF
    ENDFOREACH
    IF FILE_TEST(PlChi2_FIT) EQ 0 THEN BEGIN
        MESSAGE, "Error! "+PlChi2_FIT+" is not found!", /CONTINUE
        Check_OK = 0
    ENDIF
    IF Check_OK EQ 0 THEN MESSAGE, "Error! Please check the above error information! Exit!"
    
    ; Read InputLIB, check how many parameters
    PlChi2_NPars = []
    FOREACH Temp_LIB, PlChi2_LIB DO BEGIN 
        PlChi2_NPars = [ PlChi2_NPars, FIX(CrabTableReadInfo(Temp_LIB,"# NPAR")) ]
    ENDFOREACH
    PRINT, "Number of LIB files = "+STRING(FORMAT='(I0)',N_ELEMENTS(PlChi2_NPars))
    PRINT, "Number of LIB params = "+STRING(FORMAT='(I0)',TOTAL(PlChi2_NPars))
    
    ; Read FIT file (the chi2 output file from michi2)
    ; The FIT file starts with two columns i0 and chi2, 
    ; then for each LIB, there has two columns iLib and aLib
    ;;Verbose = 1 ;;<20160716> checked good no problem
    PlChi2_Col_i0 = LONG(CrabTableReadColumn(PlChi2_FIT,1,Verbose=Verbose,/FastReadDataBlock))
    PlChi2_Col_chi2 = DOUBLE(CrabTableReadColumn(PlChi2_FIT,2,Verbose=Verbose,/FastReadDataBlock))
    ;;CrabTablePrintC, "example/temp_i0.txt", PlChi2_Col_i0 ;;<20160716> checked good no problem
    ;;CrabTablePrintC, "example/temp_chi2.txt", PlChi2_Col_chi2 ;;<20160716> checked good no problem
    
    
    
    ; Check input_redshift.sm
    IF N_ELEMENTS(Source) EQ 0 THEN BEGIN
        IF NOT FILE_TEST("input_redshift.sm") THEN MESSAGE, 'Error! "input_redshift.sm" was not found! Please prepare this file or give Redshift from command line!'
        Source = (CrabTableReadInfo("input_redshift.sm","set source"))
    ENDIF
    IF N_ELEMENTS(Redshift) EQ 0 THEN BEGIN
        IF NOT FILE_TEST("input_redshift.sm") THEN MESSAGE, 'Error! "input_redshift.sm" was not found! Please prepare this file or give Redshift from command line!'
        Redshift = Double(CrabTableReadInfo("input_redshift.sm","set z"))
    ENDIF
    IF N_ELEMENTS(Distance) EQ 0 THEN BEGIN
        IF NOT FILE_TEST("input_redshift.sm") THEN MESSAGE, 'Error! "input_redshift.sm" was not found! Please prepare this file or give Redshift from command line!'
        Distance = Double(CrabTableReadInfo("input_redshift.sm","set dL"))
    ENDIF
    
    
    ; Read PlChi2_DAT
    readcol, PlChi2_DAT, format='(D,D,D)', wobs, fobs, dfobs
    
    
    
    ; Prepare an array for best fitting i0 chi2 i1 a1 i2 a2 etc
    PlChi2_SED_Struct = { i0:0L, chi2:0D, $
                          w:PTR_NEW(/ALLOCATE), f:PTR_NEW(/ALLOCATE), $ ; total SED
                          iLib:REPLICATE(0L,N_ELEMENTS(PlChi2_NPars)), aLib:REPLICATE(0D,N_ELEMENTS(PlChi2_NPars)), $
                          tPar:PTRARR(/ALLOCATE,N_ELEMENTS(PlChi2_NPars)), aPar:PTRARR(/ALLOCATE,N_ELEMENTS(PlChi2_NPars)), $
                          wLib:PTRARR(/ALLOCATE,N_ELEMENTS(PlChi2_NPars)), fLib:PTRARR(/ALLOCATE,N_ELEMENTS(PlChi2_NPars)), $ ; We should not use REPLICATE to create PTR array! Use PTRARR!
                          Mstar:0D, Mdust:0D, Mdust_Cold:0D, Mdust_Warm:0D, LIR:0D, SFR:0D, U:0D, Tdust:0D, qIR:0D, LAGN:0D, fAGN:0D }
    PlChi2_SED_sorted_id = SORT(PlChi2_Col_chi2) ;<TODO> select and plot 5 SEDs with best chi2
    PlChi2_SED_colors = [ '1c1ae4'xL,$
                          'b87e37'xL,$
                          '4aaf4d'xL,$
                          'a34e98'xL,$
                          '007fff'xL,$
                          '33ffff'xL,$
                          '2856a6'xL,$
                          'bf81f7'xL ] ; BGR
    PlChi2_SED_List = REPLICATE(PlChi2_SED_Struct,5) ;<TODO> select and plot 5 SEDs with best chi2
    PlChi2_SED_Id = 0
    
    
    ; LOOP
    FOR h=0, N_ELEMENTS(PlChi2_SED_sorted_id)-1 DO BEGIN
        
        sid = PlChi2_SED_sorted_id[h]
        
        PRINT, "Loading solution "+STRING(FORMAT='(I0)',sid)+"/"+STRING(FORMAT='(I0)',N_ELEMENTS(PlChi2_Col_chi2))+" chi2="+STRING(FORMAT='(F0.6)',PlChi2_Col_chi2[sid])
        
        warray = CrabArrayIndGen(-1.5,6.5,0.02)
        warray = 10^warray
        farray = MAKE_ARRAY(N_ELEMENTS(warray),N_ELEMENTS(PlChi2_NPars),/DOUBLE,VALUE=!VALUES.D_NAN)
        ftotal = MAKE_ARRAY(N_ELEMENTS(warray),/DOUBLE,VALUE=0.0D)
        
        status = 0
        
        FOR i=0, N_ELEMENTS(PlChi2_NPars)-1 DO BEGIN
            ; read fit.out
            ; which contains the entry line of each lib
            PlChi2_Col_iLib = LONG(CrabTableReadColumn(PlChi2_FIT,2+2*i+1))
            PlChi2_Col_aLib = DOUBLE(CrabTableReadColumn(PlChi2_FIT,2+2*i+1+1))
            ;;PlChi2_Col_iLib = LONG(CrabTableReadColumn(PlChi2_FIT,2+i*2+1,Verbose=Verbose,/FastReadDataBlock))
            ;;PlChi2_Col_aLib = LONG(CrabTableReadColumn(PlChi2_FIT,2+i*2+2,Verbose=Verbose,/FastReadDataBlock))
            ; <TODO> make sure aLib factor is positive <TODO>
            IF PlChi2_Col_aLib[sid] LE 0 THEN BEGIN status = -1 & BREAK & ENDIF
            ; read lib.SED
            ; according to the entry line
            IF KEYWORD_SET(Verbose) THEN PRINT, "Reading LIB "+PlChi2_LIB[i]+" i "+STRING(FORMAT='(I0)',PlChi2_Col_iLib[sid])
            ; read NVAR
            PlChi2_NVAR1 = LONG(CrabTableReadInfo(PlChi2_LIB[i],"# NVAR1"))
            ;PRINT, "PlChi2_NVAR1 = ", PlChi2_NVAR1 ; this is the wavelength axis dimension
            ; print message
            IF KEYWORD_SET(Verbose) THEN PRINT, "Reading LIB "+PlChi2_LIB[i]+" i "+STRING(FORMAT='(I0)',PlChi2_Col_iLib[sid])+"--"+STRING(FORMAT='(I0)',PlChi2_Col_iLib[sid]+PlChi2_NVAR1-1)
            ; read CVAR1
            PlChi2_CVAR1 = LONG(CrabTableReadInfo(PlChi2_LIB[i],"# CVAR1"))
            ;PRINT, "PlChi2_CVAR1 = ", PlChi2_CVAR1 ; this is the wavelength axis data column number
            PlChi2_CVAR1_DATA = CrabTableReadColumn(PlChi2_LIB[i],PlChi2_CVAR1,Verbose=Verbose,/FastReadDataBlock) ; wavelength data
            PlChi2_CVAR1_DATA = PlChi2_CVAR1_DATA[PlChi2_Col_iLib[sid]:PlChi2_Col_iLib[sid]+PlChi2_NVAR1-1]
            IF KEYWORD_SET(Verbose) THEN PRINT, "PlChi2_CVAR1_DATA = ", PlChi2_CVAR1_DATA[0:2]
            IF KEYWORD_SET(Verbose) THEN PRINT, "PlChi2_CVAR1_DATA = ", PlChi2_CVAR1_DATA[PlChi2_NVAR1-3:PlChi2_NVAR1-1]
            ; read CVAR2
            PlChi2_CVAR2 = LONG(CrabTableReadInfo(PlChi2_LIB[i],"# CVAR2"))
            ;PRINT, "PlChi2_CVAR2 = ", PlChi2_CVAR2 ; this is the flux axis data column number
            PlChi2_CVAR2_DATA = CrabTableReadColumn(PlChi2_LIB[i],PlChi2_CVAR2,Verbose=Verbose,/FastReadDataBlock) ; flux data
            PlChi2_CVAR2_DATA = PlChi2_CVAR2_DATA[PlChi2_Col_iLib[sid]:PlChi2_Col_iLib[sid]+PlChi2_NVAR1-1]
            IF KEYWORD_SET(Verbose) THEN PRINT, "PlChi2_CVAR2_DATA = ", PlChi2_CVAR2_DATA[0:2]
            IF KEYWORD_SET(Verbose) THEN PRINT, "PlChi2_CVAR2_DATA = ", PlChi2_CVAR2_DATA[PlChi2_NVAR1-3:PlChi2_NVAR1-1]
            
            wtempo = DOUBLE(PlChi2_CVAR1_DATA)
            ftempo = DOUBLE(PlChi2_CVAR2_DATA)*DOUBLE(PlChi2_Col_aLib[sid])
            itempo = WHERE(warray GE MIN(wtempo) AND warray LE MAX(wtempo), /NULL)
            IF N_ELEMENTS(itempo) GT 0 THEN BEGIN
                ftemps = INTERPOL(ftempo, wtempo, warray[itempo], /SPLINE)
                farray[itempo,i] = (ftemps)
                ftotal[itempo] = (ftotal[itempo] + ftemps)
            ENDIF
            
            ; <TODO> compute physical parameters for this solution
            Temp_iLib = PlChi2_Col_iLib[sid]
            Temp_aLib = PlChi2_Col_aLib[sid]
            Temp_chi2 = PlChi2_Col_chi2[sid]
            Temp_tPar = MAKE_ARRAY(PlChi2_NPars[i],/STRING) ; parameter title array
            Temp_aPar = MAKE_ARRAY(PlChi2_NPars[i],/DOUBLE) ; parameter value array
            Temp_wLib = wtempo
            Temp_fLib = ftempo
            FOR j=0, PlChi2_NPars[i]-1 DO BEGIN
                Temp_iPar = FIX(CrabTableReadInfo(PlChi2_LIB[i],"# CPAR"+STRING(FORMAT='(I0)',j+1)))
                Temp_tPar[j] = CrabTableReadInfo(PlChi2_LIB[i],"# TPAR"+STRING(FORMAT='(I0)',j+1))
                Temp_aPar[j] = DOUBLE(CrabTableReadColumn(PlChi2_LIB[i],Temp_iPar,SkipValidLines=PlChi2_Col_iLib[sid],ReadValidLines=1,Verbose=Verbose))
            ENDFOR
            ; ***********************************
            ; * Compute stellar mass 
            ; ***********************************
            IF STRMATCH(PlChi2_LIB[i],'FSPS.Padova.*.SED') THEN BEGIN
                IF Temp_tPar[2] EQ 'Mass' THEN BEGIN
                    PlChi2_SED_List[PlChi2_SED_Id].Mstar = Temp_aPar[2] * Temp_aLib / (1.0+Redshift) ; 1+z is because of rShift.sm
                    PRINT, "Computing stellar mass   "+STRING(FORMAT='(E0.6)', PlChi2_SED_List[PlChi2_SED_Id].Mstar)+" [Msun] (z="+STRING(FORMAT='(F0.4)',Redshift)+")"
                ENDIF
            ENDIF
            ; ***********************************
            ; * Compute dust mass 
            ; ***********************************
            IF STRMATCH(PlChi2_LIB[i],'DL07.HiExCom.*.SED') THEN BEGIN
                PlChi2_SED_List[PlChi2_SED_Id].Mdust_Warm = Temp_aLib * Distance^2 / (1.0+Redshift) ; 1+z is because of rShift.sm
                PRINT, "Computing warm dust mass "+STRING(FORMAT='(E0.6)', Temp_aLib * Distance^2 / (1.0+Redshift))+" [Msun] (z="+STRING(FORMAT='(F0.4)',Redshift)+")"+" (a="+STRING(FORMAT='(E0.6)',Temp_aLib)+")"
            ENDIF
            IF STRMATCH(PlChi2_LIB[i],'DL07.LoExCom.*.SED') THEN BEGIN
                PlChi2_SED_List[PlChi2_SED_Id].Mdust_Cold = Temp_aLib * Distance^2 / (1.0+Redshift) ; 1+z is because of rShift.sm
                PRINT, "Computing cold dust mass "+STRING(FORMAT='(E0.6)', Temp_aLib * Distance^2 / (1.0+Redshift))+" [Msun] (z="+STRING(FORMAT='(F0.4)',Redshift)+")"+" (a="+STRING(FORMAT='(E0.6)',Temp_aLib)+")"
            ENDIF
            ; ***********************************
            ; * Compute AGN luminosity
            ; ***********************************
            IF STRMATCH(PlChi2_LIB[i],'MullaneyAGN.*.SED') THEN BEGIN
                ;Temp_iTot = WHERE((Temp_wLib GE 3.0*(1.0+Redshift)) AND (Temp_wLib LE 1000.0*(1.0+Redshift)),/NULL)
                ;Temp_SAGN = TOTAL(Temp_fLib * (2.99792458e5/Temp_wLib) * Temp_wBin / ALOG10(exp(1)) )           ; integrated flux in mJy GHz -- daddi method
                ;Temp_SAGN = TOTAL(Temp_fLib[Temp_iTot] * (2.99792458e5/Temp_wLib[Temp_iTot]) * (10^Temp_wBin-(1.0/10^Temp_wBin))/2.0 ) ; integrated flux in mJy GHz -- dzliu method <TODO> Temp_wBin for wLib is not the same one as wTot!
                Temp_freq = CrabArrayIndGen(MIN(2.99792458e5/Temp_wLib),MAX(2.99792458e5/Temp_wLib),1.0)
                Temp_flog = Interpol(ALOG10(Temp_fLib),ALOG10(2.99792458e5/Temp_wLib),ALOG10(Temp_freq))
                Temp_SAGN = TOTAL(10^(Temp_flog)) ; mJy GHz -- dzliu stupid method
                ;PRINT, Temp_SAGN
                PlChi2_SED_List[PlChi2_SED_Id].LAGN = Temp_SAGN / 1D20 * 4*!PI*Distance^2 * 9.52140D44 / 3.839D26 ; 1D20 converts mJy GHz to W m-2, 9.52140D44 converts Mpc^2 to m^2. 
                PRINT, "Computing AGN luminosity "+STRING(FORMAT='(E0.6)', PlChi2_SED_List[PlChi2_SED_Id].LAGN)+" [Lsun] (z="+STRING(FORMAT='(F0.4)',Redshift)+")"
            ENDIF
        
            ; Save to SED data structure
             (PlChi2_SED_List[PlChi2_SED_Id].i0) = sid
             (PlChi2_SED_List[PlChi2_SED_Id].chi2) = Temp_chi2
             (PlChi2_SED_List[PlChi2_SED_Id].iLib[i]) = Temp_iLib
             (PlChi2_SED_List[PlChi2_SED_Id].aLib[i]) = Temp_aLib
            *(PlChi2_SED_List[PlChi2_SED_Id].tPar[i]) = Temp_tPar
            *(PlChi2_SED_List[PlChi2_SED_Id].aPar[i]) = Temp_aPar
            *(PlChi2_SED_List[PlChi2_SED_Id].wLib[i]) = warray
            *(PlChi2_SED_List[PlChi2_SED_Id].fLib[i]) = farray
            
        ENDFOR
        ; 
        ; Check status
        IF status LT 0 THEN BEGIN CONTINUE & ENDIF
        ; 
        ; Compute LIR and SFR
        IF 1 EQ 1 THEN BEGIN
            Temp_wTot = warray
            Temp_fTot = ftotal
            Temp_iTot = WHERE((Temp_wTot GE 8.0*(1.0+Redshift)) AND (Temp_wTot LE 1000.0*(1.0+Redshift)),/NULL)
            ;Temp_fTIR = TOTAL(Temp_fTot[Temp_iTot] * (2.99792458e5/Temp_wTot[Temp_iTot]) * Temp_wBin / ALOG10(exp(1)) )           ; integrated flux in mJy GHz -- daddi method
            ;Temp_fTIR = TOTAL(Temp_fTot[Temp_iTot] * (2.99792458e5/Temp_wTot[Temp_iTot]) * (10^Temp_wBin-(1.0/10^Temp_wBin))/2.0 ) ; integrated flux in mJy GHz
            Temp_freq = CrabArrayIndGen(MIN(2.99792458e5/Temp_wTot[Temp_iTot]),MAX(2.99792458e5/Temp_wTot[Temp_iTot]),1.0)
            Temp_flog = Interpol(ALOG10(Temp_fTot[Temp_iTot]),ALOG10(2.99792458e5/Temp_wTot[Temp_iTot]),ALOG10(Temp_freq))
            Temp_fTIR = TOTAL(10^(Temp_flog)) ; mJy GHz -- dzliu stupid method
            ;PRINT, Temp_fTIR
            PlChi2_SED_List[PlChi2_SED_Id].LIR = Temp_fTIR / 1D20 * 4*!PI*Distance^2 * 9.52140D44 / 3.839D26 ; 1D20 converts mJy GHz to W m-2, 9.52140D44 converts Mpc^2 to m^2. 
            PlChi2_SED_List[PlChi2_SED_Id].SFR = PlChi2_SED_List[PlChi2_SED_Id].LIR / 1D10 ; Chabrier (2003) IMF
                PRINT, "Computing IR luminosity  "+STRING(FORMAT='(E0.6)', PlChi2_SED_List[PlChi2_SED_Id].LIR)+" [Lsun] (z="+STRING(FORMAT='(F0.4)',Redshift)+")"
        ENDIF
        ; 
        ; Compute Mdust
        IF 1 EQ 1 THEN BEGIN
            PlChi2_SED_List[PlChi2_SED_Id].Mdust = PlChi2_SED_List[PlChi2_SED_Id].Mdust_Warm + PlChi2_SED_List[PlChi2_SED_Id].Mdust_Cold
        ENDIF
        ; 
        ; Compute Umean
        IF 1 EQ 1 THEN BEGIN
            PlChi2_SED_List[PlChi2_SED_Id].U = PlChi2_SED_List[PlChi2_SED_Id].LIR / PlChi2_SED_List[PlChi2_SED_Id].Mdust / 125.0D
        ENDIF
        ; 
        ; Compute Tdust
        IF 1 EQ 1 THEN BEGIN
            PlChi2_SED_List[PlChi2_SED_Id].Tdust = 17.0D * (PlChi2_SED_List[PlChi2_SED_Id].U)^(1.0D/6.0D)
        ENDIF
        ; 
        ; Add radio flux SED by computing L_IR_8_1000
        ;;IF 1 EQ 1 THEN BEGIN
        ;;    Temp_wTot = warray
        ;;    Temp_fTot = ftotal
        ;;    Temp_iTot = WHERE((Temp_wTot GE 8) AND (Temp_wTot LE 1000),/NULL)
        ;;    Temp_fTIR = TOTAL(Temp_fTot[Temp_iTot] * (2.99792458e5/Temp_wTot[Temp_iTot]) * Temp_wBin / ALOG10(exp(1)) ) ; integrated flux in mJy GHz
        ;;    Temp_wRadio = Temp_wTot
        ;;    Temp_fRadio = Temp_fTIR*1e9 / 3.75e12 / 10^2.4 * ((2.99792458e5/Temp_wTot)/1.4)^(-0.80) ; # 3.75e12 is from arxiv.org/pdf/1005.1072, 10^2.4 is 10^qIR. 
        ;;    ;;OPLOT, Temp_wRadio, Temp_fRadio, Color=cgColor(PlotColors[N_ELEMENTS(PlotColors)-1]), LINESTYLE=1
        ;;    ;;Temp_fTot = Temp_fTot + Temp_fRadio
        ;;ENDIF
        ; 
        ; Compute qIR
        IF 1 EQ 1 THEN BEGIN
            Temp_wTot = warray
            Temp_fTot = ftotal
            Temp_wBin = 0.01 ; ALOG10(warray[1])-ALOG10(warray[0])
            Temp_iTot = WHERE((Temp_wTot GE 8) AND (Temp_wTot LE 1000),/NULL)
            Temp_fTIR = TOTAL(Temp_fTot[Temp_iTot] * (2.99792458e5/Temp_wTot[Temp_iTot]) * Temp_wBin / ALOG10(exp(1)) ) ; integrated flux in mJy GHz
            Temp_wRadio = 2.99792458e5 / 1.4D ; 1.4GHz = 2e5um
            Temp_fRadio = 10^(Interpol(ALOG10(Temp_fTot),ALOG10(Temp_wTot),ALOG10(Temp_wRadio)))
            PlChi2_SED_List[PlChi2_SED_Id].qIR = ALOG10(Temp_fTIR*1e9 / 3.75e12 / Temp_fRadio) ; # 3.75e12 is from arxiv.org/pdf/1005.1072
        ENDIF
        ; 
        ; 
        ; 
        ; plot SED
        IF PlChi2_SED_Id EQ 0 THEN BEGIN
            PlotDevice = PLOT(warray*(1.0+Redshift), ftotal, /XLOG, /YLOG, /NODATA, XTITLE='Observing Wavelength [um]', YTITLE='Flux Density [mJy]')
            PlotDevice = PLOT(warray*(1.0+Redshift), farray[*,0], COLOR='green',/OVERPLOT)
            PlotDevice = PLOT(warray*(1.0+Redshift), farray[*,1], COLOR='yellow',/OVERPLOT)
            PlotDevice = PLOT(warray*(1.0+Redshift), farray[*,2], COLOR='red',/OVERPLOT)
            PlotDevice = PLOT(warray*(1.0+Redshift), farray[*,3], COLOR='blue',/OVERPLOT)
            PlotDevice = PLOT(warray*(1.0+Redshift), farray[*,4], COLOR='magenta',/OVERPLOT)
            PlotDevice = PLOT(warray*(1.0+Redshift), ftotal, /OVERPLOT)
            PlotDevice = PLOT(wobs, fobs, SYMBOL='square', LINESTYLE=' ', THICK=3, /OVERPLOT)
            PlotDevice = ERRORPLOT(wobs, fobs, dfobs, LINESTYLE=' ', THICK=3, /OVERPLOT)
        ENDIF
        ; 
        ; 
        ; 
        ; break 
        IF PlChi2_SED_Id EQ 4 THEN BEGIN
            BREAK
        ENDIF ELSE BEGIN
            PlChi2_SED_Id = PlChi2_SED_Id + 1
        ENDELSE
        
    ENDFOR
    
    
    
    
END