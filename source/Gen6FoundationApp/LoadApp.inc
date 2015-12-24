ERROR=ABORT
WAIT=ON

RESET

//Load the Boot AXF File
load/r 'C:\Sanboxes\iMX25_Gen6_Illumination\Gen6FoundationApp\Output_Boot_imx25_iram_evalbd_bd\db_app.axf'

go		//Run Boot
halt		//Stop, This will only work when you also stop boot
unload 1	//Unload the Boot Program 
delfile 1	//Deleate it from the debugger

//Load the App AXF File

load/r 'C:\Sanboxes\iMX25_Gen6_Illumination\Gen6FoundationApp\Output_App_imx25_evalbd\db_app.axf'

printf "Your App is Now Loaded"

//go		//Run App
//		//Load Some Break points
