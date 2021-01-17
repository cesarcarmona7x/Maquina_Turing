#include "stdafx.h"
void Parser::parseString(HWND hwnd,std::wstring str){
	SendMessage(hwnd,WM_STARTTAPE,0,0);
	characters.clear();
	int countm=0;
	int countn=0;
	bool trono=false;
	for(int i=0;i<str.length();i++){
		std::wstring stat(std::wstring(L"Añadiendo caracter en posición ")+std::to_wstring(i));
		SendMessage(hwnd,WM_CHANGESTATUS,0,(LPARAM)_wcsdup(stat.c_str()));
		characters.push_back(str.at(i));
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		SendMessage(hwnd,WM_ADDLETTER,i,0);
		SendMessage(hwnd,WM_MOVEMARKER,0,0);
		switch(str.at(i)){
		case L'1':
			countm++;
			break;
		case L'A':
			countn++;
			break;
		case L'b':
			break;
		default:
			trono=true;
		}
		if(trono)break;
	}
	int remainingm=countm;
	if(trono){
		SendMessage(hwnd,WM_CHANGESTATUS,0,(LPARAM)L"Caracteres no válidos.");
	}
	else{
		SendMessage(hwnd,WM_CHANGESTATUS,0,(LPARAM)L"Rebobinando cinta...");
		for(int i=str.length();i>0;i--){
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			SendMessage(hwnd,WM_RETURNMARKER,0,0);
		}

		if(countm<=countn){
			SendMessage(hwnd,WM_CHANGESTATUS,0,(LPARAM)L"Compilando...");
			str=L"";
			for(int i=0;i<characters.size();i++){
				if(remainingm>0){
					if(characters.at(i)==L'A'){
						characters.at(i)=L'B';
						remainingm--;
						SendMessage(hwnd,WM_UPDATELETTER,i,0);
					}
				}
				str.push_back(characters.at(i));
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				SendMessage(hwnd,WM_MOVEMARKER,0,0);
			}
			SendMessage(hwnd,WM_CHANGESTATUS,0,(LPARAM)L"Finalizado.");
		}
		else{
			SendMessage(hwnd,WM_CHANGESTATUS,0,(LPARAM)L"M es mayor que N.");
		}
	}
	SendMessage(hwnd,WM_ENDTAPE,0,0);
}