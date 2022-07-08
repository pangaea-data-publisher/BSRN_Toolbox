#ifndef _GLOBALS

    #define _GLOBALS

// Application constants

   #if defined(Q_OS_LINUX)
       const char eol = '\n';
   #endif

   #if defined(Q_OS_WIN)
       const char eol = '\n';
   #endif

   #if defined(Q_OS_MAC)
       const char eol = '\r';
   #endif

    const QString   _PROGRAMDOI_        = "doi:10.1594/PANGAEA.774827";
    const QString   _PROGRAMREADME_     = "https://wiki.pangaea.de/wiki/BSRN_Toolbox";

    const QString   _VERSION_           = "2.9";                       // Version number, 2017-10-16
    const QString   _DOI_               = "doi:10.1594/PANGAEA.774827"; // Dataset

    // EOL
    const int   _UNIX_                  = 0;    // LF
    const int   _MACOS_                 = 1;    // CR
    const int   _WIN_                   = 2;    // CR+LF
    const int   _WINMAC_                = 3;    // CR

    // Encoding
    const int   _SYSTEM_                = -1;   // System
    const int   _UTF8_                  = 0;    // UTF-8
    const int   _APPLEROMAN_            = 1;    // Apple Roman
    const int   _LATIN1_                = 2;    // Latin-1 = ISO 8859-1

    // Extension
    const int   _TXT_                   = 0;
    const int   _CSV_                   = 1;

    const int	_NOERROR_               = 0;
    const int	_ERROR_					= 1;
    const int	_APPBREAK_				= 2;
    const int	_NODATAFOUND_			= 3;
    const int	_DATAFOUND_				= 4;
    const int   _CHOOSEABORTED_         = 5;
    const int   _FILENOEXISTS_          = 6;
    const int   _RECORDNOTFOUND_        = 7;

    const int   _HELP_                       = 10;
    const int   _BSRNSTATIONTOARCHIVEFORMAT_ = 11;
    const int   _BSRNSTATUS_                 = 12;
    const int   _BSRNHOMEPAGE_               = 13;
    const int   _BSRNLATESTSNAPSHOT_         = 14;
    const int   _BSRNACCOUNT_                = 15;
    const int   _BSRNCHECK_                  = 16;

    const int   FILENOTWRITABLE         = -20;
    const int   DATETIMENOTFOUND        = 1000;
    const int   LATITUDENOTFOUND        = 1001;
    const int   LONGITUDENOTFOUND       = 1002;
    const int   ELEVATIONNOTFOUND       = 1003;
    const int   HEIGHTNOTFOUND          = 1004;
    const int   SWDNOTFOUND             = 1005;
    const int   DIRNOTFOUND             = 1006;
    const int   DIFNOTFOUND             = 1007;
    const int   LWDNOTFOUND             = 1008;
    const int   SWUNOTFOUND             = 1010;
    const int   LWUNOTFOUND             = 1011;
    const int   T2NOTFOUND              = 1012;
    const int   RHNOTFOUND              = 1013;
    const int   POPOPOPONOTFOUND        = 1014;
    const int   NODATAFOUND             = 1015;

    const int	MAX_NUM_OF_METHODS		= 3500;  // 3000 26.6.2020
    const int	MAX_NUM_OF_STAFFS		= 200;   // 100 14.10.20
    const int	MAX_NUM_OF_STATIONS		= 89;   // 05.05.21, take always the exact last ID of stations!
    const int	MAX_NUM_OF_PARAMETER	= 300;
    const int	MAX_NUM_OF_MONTHS		= 12;
    const int	MAX_NUM_OF_YEARS		= 31;    // 05.05.21, take always the exact number of checkboxes in dialog!
    const int   MAX_NUM_OF_DATASETS     = 60000; // 14.10.20: 50000 2016-05-03: 21,460 Datasets in BSRN_Datasets_IDs.txt
    const int   MAX_NUM_OF_REFERENCES   = 30000; // 14.10.20: 20000 2016-05-03: 9.000 References

    const int   LR0100                  = 1;
    const int   LR0300                  = 2;
    const int   LR0100plusLR0300        = 3;
    const int   LR0500                  = 4;

    const int	_ZIP_					= 1;
    const int	_GZIP_					= 2;

    const int   _TABSTOP_               = 0;
    const int   _COMMA_                 = 1;
    const int   _SEMICOLON_             = 2;
    const int   _SPACE_                 = 3;

    const int   _RELATIONTYPE_          = 13; // other version, 2013-01-23

// Constants
    const int   ALE = 18;
    const int   ASP =  1;
    const int   BAR = 22;
    const int   BER = 24;
    const int   BIL = 28;
    const int   BON = 32;
    const int   BOS = 34;
    const int   BOU = 23;
    const int   BRB = 71;
    const int   BUD = 14;
    const int   CAB = 53;
    const int   CAM = 50;
    const int   CAP = 15; // 2017-10-16
    const int   CAR = 10;
    const int   CLH = 39;
    const int   CNR = 45;
    const int   COC = 47;
    const int   DAA = 40;
    const int   DAR =  2;
    const int   DOM = 74;
    const int   DON = 55; // 2017-10-16
    const int   DRA = 35;
    const int   DWN = 65;
    const int   E13 = 27;
    const int   ENA = 77; // 2017-01-22
    const int   EUR = 19;
    const int   FLO =  3;
    const int   FPE = 31;
    const int   FUA =  6;
    const int   GAN = 58; // 2017-10-16
    const int   GCR = 33;
    const int   GIM = 78; // 2018-10-25
    const int   GOB = 20; // 2013-01-05
    const int   GUR = 56; // 2017-10-16
    const int   GVN = 13;
    const int   HOW = 57; // 2017-10-16
    const int   ILO = 38;
    const int   ISH =  7;
    const int   IZA = 61;
    const int   KWA = 25;
    const int   LAU = 60;
    const int   LER = 51;
    const int   LIN = 12;
    const int   LLN = 54; // 2017-10-16
    const int   LRC = 49; // 2015-02-05
    const int   LYU = 79; // 2018-10-25
    const int   MAN = 29;
    const int   MNM =  8;
    const int   NAU = 30;
    const int   NEW = 52; // 2017-10-16
    const int   NYA = 11;
    const int   OHY = 80; // 2018-10-25
    const int   PAL = 63;
    const int   PAR = 66; // 2018-10-25
    const int   PAY = 21;
    const int   PSA = 76; // 2014-09-09
    const int   PSU = 36;
    const int   PTR = 72;
    const int   REG =  5;
    const int   RLM = 73;
    const int   RUN = 82; // 2018-10-25
    const int   SAP =  4;
    const int   SBO = 43;
    const int   SEL = 83; // 2018-10-25
    const int   SMS = 70;
    const int   SON = 75; // 2013-03-06
    const int   SOV = 41;
    const int   SPO = 26;
    const int   SXF = 37;
    const int   SYO = 17;
    const int   TAM = 42;
    const int   TAT = 16;
    const int   TIK = 48;
    const int   TIR = 59; // 2017-10-16
    const int   TOR =  9;
    const int   XIA = 44;
    const int   YUS = 84; // 2018-10-25
    //const int   ZVE = 46;
    const int   CYL = 85; // 2021-04-30
    const int   ABS = 86; // 2021-04-30
    const int   NPT = 87; // 2021-04-30
    const int   INO = 88; // 2021-04-30
    const int   TNB = 89; // 2021-04-30
#endif
