///////////////////////////////////////////////////////////////////////////////
// Name: Chip 8 Interpreter
//
// Author: Jonathan Del Corpo
// Contact: jonathan_delcorpo@hotmail.com
//
// License: GNU General Public License (GPL) v2 
// ( http://www.gnu.org/licenses/old-licenses/gpl-2.0.html )
//
///////////////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <string>
#include <fstream>
#include <cstring>
#include <cstddef>
#include <iostream>
#include <time.h>
#include <SDL.h>

class Chip8
{
	//35 opcodes, 2 bytes each
	unsigned short opcode;

	//4k memory
	/*
	** MEMORY MAP:
	** 0x000 - 0x1FF - Chip 8 interpreter (contains font set in emu)
	** 0x050 - 0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	** 0x200 - 0xFFF - Program ROM and work RAM
	*/
	unsigned char memory[4096];

	//CPU registers, 15 8-bit general purpose registers (V0-VE)(VF carry flag)
	unsigned char V[16];

	//Index register andprogram counter (values from 0x000 to 0xFFF)
	unsigned short I;
	unsigned short pc;

	//Timer registers 60HZ
	unsigned char delay_timer;
	unsigned char sound_timer;


	//Stack used to remember the location before a jump
	unsigned short stack[16];

	//Stack pointer
	unsigned short sp;

	//Chip 8 fontset
	unsigned char chip8_fontset[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};


	//opcode funtions (35 opcodes)//
	void SYS();		//00E0 - SYS addr
	void CLS();		//00E0 - CLS
	void RET();		//00EE - RET
	void JP();		//1nnn - JP addr
	void CALL();	//2nnn - CALL addr
	void SE();		//3xkk - SE Vx, byte
	void SNE();		//4xkk - SNE Vx, byte
	void SE2();		//5xy0 - SE Vx, Vy
	void LD();		//6xkk - LD Vx, byte
	void ADD();		//7xkk - ADD Vx, byte
	void LD2();		//8xy0 - LD Vx, Vy
	void OR();		//8xy1 - OR Vx, Vy
	void AND();		//8xy2 - AND Vx, Vy
	void XOR();		//8xy3 - XOR Vx, Vy
	void ADD2();	//8xy4 - ADD Vx, Vy
	void SUB();		//8xy5 - SUB Vx, Vy
	void SHR();		//8xy6 - SHR Vx {, Vy}
	void SUBN();	//8xy7 - SUBN Vx, Vy
	void SHL();		//8xyE - SHL Vx {, Vy}
	void SNE2();	//9xy0 - SNE Vx, Vy
	void LD3();		//Annn - LD I, addr
	void JP2();		//Bnnn - JP V0, addr
	void RND();		//Cxkk - RND Vx, byte
	void DRW();		//Dxyn - DRW Vx, Vy, nibble
	void SKP();		//Ex9E - SKP Vx
	void SKNP();	//ExA1 - SKNP Vx
	void LD4();		//Fx07 - LD Vx, DT
	void LD5();		//Fx0A - LD Vx, K
	void LD6();		//Fx15 - LD DT, Vx
	void LD7();		//Fx18 - LD ST, Vx
	void ADD3();	//Fx1E - ADD I, Vx
	void LD8();		//Fx29 - LD F, Vx
	void LD9();		//Fx33 - LD B, Vx
	void LD10();	//Fx55 - LD [I], Vx
	void LD11();	//Fx65 - LD Vx, [I]
	/////////////////////////////////////////

	void movePC();

	void clearGFX();


	
	 
public:
	Chip8();
	~Chip8();

	//Graphics (64 x 32 pixels) 2048 pixels.
	//Pixels can either be black or white (0 or 1)
	unsigned char gfx[32][64];

	//Keypad state (Hex based 0x0 - 0xF) stores current state of the key
	unsigned char key[16];

	void initialize();

	void executeCycle();

	void loadGame(std::string gamePath);

	unsigned char getDelayTimer();
};

