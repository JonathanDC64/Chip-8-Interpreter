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
#include "Chip8.h"

Chip8::Chip8() {}

Chip8::~Chip8() {}

void Chip8::initialize()
{
	// Initialize registers and memory once

	pc = 0x200;	// Program counter starts at 0x200
	opcode = 0;		// Reset current opcode
	I = 0;		// Reset index register
	sp = 0;		// Reset stack pointer

				// Clear display
	clearGFX();

	// Clear stack
	for (int i = 0; i < 16; ++i)
		stack[i] = 0;

	// Clear keys  and registers
	for (int i = 0; i < 16; ++i)
		key[i] = V[i] = 0;

	// Clear memory
	for (int i = 0; i < 4096; ++i)
		memory[i] = 0;

	// Load fontset
	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;

	srand(time(NULL));
}

// For opcodes:
// https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#00E0
void Chip8::executeCycle()
{
	// Fetch Opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	printf("opcode: %X pc: %d\n", opcode, pc);

	// Decode Opcode
	switch (opcode & 0xF000) // first 4 bits of the opcode
	{
		// Some opcodes

		// If first 4 bits start with 0
		// For 0x0XXX there are 2 opcodes
	case 0x0000:

		// Last 4 bits
		switch (opcode & 0x000F)
		{
			// 0x00E0: Clears the screen - CLS
		case 0x0000:
			CLS();
			break;

			// 0x00EE: Returns from subroutine - RET
		case 0x000E:
			RET();
			break;

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
			break;
		}

		break;

		// For 0x1XXX there is 1 opcode
		// 0x1NNN jumps to memory location nnn
	case 0x1000:
		JP();
		break;

		// For 0x2XXX there is 1 opcode
		// 0x2NNN - calls subroutine at address NNN - CALL
	case 0x2000:
		CALL();
		break;

		// For 0x3XXX there is 1 opcode
		// 0x3XNN - skips next instruction if VX is equal to NN
	case 0x3000:
		SE();
		break;

		// For 0x4XXX there is 1 opcode
		// 0x4XNN - skips next instruction if VX is not equal to NN
	case 0x4000:
		SNE();
		break;

		// For 0x5XXX there is 1 opcode
		// 0x5XY0 - skips next instruction if VX and VY are equal
	case 0x5000:
		SE2();
		break;

		// For 0x6XXX there is 1 opcode
		// 0x6XNN - set VX to NN
	case 0x6000:
		LD();
		break;

		// For 0x7XXX there is 1 opcode
		// 0x7XNN - add NN to VX
	case 0x7000:
		ADD();
		break;

		// For 0x8XXX there are 9 opcodes
	case 0x8000: // 0x8XY[0-E] cpu rewgister manipulation opcodes
		switch (opcode & 0x000F)
		{
			// 8xy0 - Set VX to VY - LD
		case 0x0000:
			LD2();
			break;

			// 8xy1 - Set VX to VX or VY - OR
		case 0x0001:
			OR();
			break;

			// 8xy2 - Set VX to VX and VY - AND
		case 0x0002:
			AND();
			break;

			// 8xy2 - Set VX to VX xor VY - XOR
		case 0x0003:
			XOR();
			break;

			// Adds VY to VX - ADD
			// 8xy4 - Set VF to 1 for carry (if sum is larger than 255 or 0xF0) 0 when there isn't
		case 0x0004:
			ADD2();
			break;

			// Substracts VY from VX - SUB
			// 8xy5 - Set VF to 1 for borrow (if subraction is less than 0 or 0x00) 0 when there isn't
		case 0x0005:
			SUB();
			break;

			// Shift  VX right by 1 - SHR
			// 8xy6 - VF is set to the value of the least significant bit of VX before the shift
		case 0x0006:
			SHR();
			break;

			// 8xy7 - Subtracts VX from VY and stores the result in VX - SUBN
		case 0x0007:
			SUBN();
			break;

			// Shift  VX left by 1 - SHL
			// VF is set to the value of the least significant bit of VX before the shift
		case 0x000E:
			SHL();
			break;

		default:
			printf("Unknown opcode [0x8XY-]: 0x%X\n", opcode);
			break;
		}
		break;

		// For 0x9XXX there is 1 opcode
		// 0x9XY0 - skip to next instruction if VX is not equal to VY
	case 0x9000:
		SNE2();
		break;

		// For 0xAXXX there is 1 opcode
		// 0xANNN - Sets I to the address NNN (Last 12 bits)
	case 0xA000:
		LD3();
		break;

		// For 0xBXXX there is 1 opcode
		// 0xBNNN - Jump to location NNN + V0
	case 0xB000:
		JP2();
		break;

		// For 0xCXXX there is 1 opcode
		// Cxnn - Set Vx equal to random byte AND nn.
	case 0xC000:
		RND();
		break;

		// For 0xDXXX there is 1 opcode
		// 0xDXYN - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
	case 0xD000:
		DRW();
		break;

		// For 0xEXXX there are 2 opcodes
	case 0xE000:
		switch (opcode & 0x00FF)
		{
			// 0xEX9E - skip to next instruction if key with the value VX is pressed
		case 0x009E:
			SKP();
			break;

			// 0xA1 - Skip next instruction if key with the value of Vx is not pressed.
		case 0x00A1:
			SKNP();
			break;

		default:
			printf("Unknown opcode [0xEX--]: 0x%X\n", opcode);
			break;
		}
		break;

		//For 0xFXXX there are 9 opcodes
	case 0xF000:
		switch (opcode & 0x00FF)
		{
			// 0xFX07 - Set VX to delay timer value
		case 0x0007:
			LD4();
			break;

			// 0xFX0A - Wait for a key press, store the value of the key in VX
		case 0x000A:
			LD5();
			break;

			// 0xFX15 - Set delay timer to VX
		case 0x0015:
			LD6();
			break;

			// 0xFX18 - set sound timer equal to VX
		case 0x0018:
			LD7();
			break;

			// 0xFX1E - Add I to VX and store in I
		case 0x001E:
			ADD3();
			break;

			// 0xFX29 - Set I to location of sprite for digit VX
		case 0x0029:
			LD8();
			break;

			// 0xFX33 - Store BCD representation of Vx in memory locations I, I + 1, and I + 2.
		case 0x0033:
			LD9();
			break;

			// 0xFX55 - Store registers V0 through VX in memory starting at location I
		case 0x0055:
			LD10();
			break;

			// 0xFX65 - Read registers V0 through Vx from memory starting at location I.
		case 0x0065:
			LD11();
			break;

		default:
			printf("Unknown opcode [0xFX--]: 0x%X\n", opcode);
			break;
		}
		break;

	default:
		printf("Unknown opcode: 0x%X\n", opcode);
		break;
	}

	// Execute Opcode

	// Update timers
	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
			printf("BEEP!\n");

		--sound_timer;
	}

	//system("pause");
}

void Chip8::loadGame(std::string gamePath)
{
	std::ifstream gameFile(gamePath, std::ios::in | std::ios::binary);
	gameFile.seekg(0, std::ios::end);
	size_t length = gameFile.tellg();
	char *bytes = new char[length];
	gameFile.seekg(0, std::ios::beg);
	gameFile.read(bytes, length);

	for (size_t i = 0; i < length; i++)
	{
		unsigned char byte = bytes[i];
		memory[512 + i] = byte;
	}

	gameFile.close();
}

unsigned char Chip8::getDelayTimer()
{
	return delay_timer;
}

// 0nnn - SYS addr
// Jump to a machine code routine at nnn.
void Chip8::SYS()
{
	// Ignored by modern interpreters
}

//00E0 - CLS
//Clear the display.
void Chip8::CLS()
{
	// There are 2048 pixels
	clearGFX();
	movePC();
}



//00EE - RET
//Return from a subroutine.
void Chip8::RET()
{
	// Pop the stack, going to the previous value
	--sp;

	// Set the program counter to the previous memory location from the stack
	pc = stack[sp];

	// Move the program counter by 2 bytes
	movePC();
}

//1nnn - JP addr
//Jump to location nnn.
void Chip8::JP()
{
	pc = opcode & 0x0FFF;
}

//2nnn - CALL addr
//Call subroutine at nnn.
void Chip8::CALL()
{
	// Push the current program counter to the stack so that we can go back later
	stack[sp] = pc;

	// Move the stack pointer by 1 for the next program counter to be pushed onto it
	++sp;

	// set the current program counter to the memory location of the subroutine NNN
	pc = opcode & 0xFFF;
}

//3xnn - SE Vx, byte
//Skip next instruction if Vx = nn.
void Chip8::SE()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char NN = opcode & 0x00FF;
	if (V[X] == NN)
		movePC();

	movePC();
}

//4xnn - SNE Vx, byte
//Skip next instruction if Vx != nn.
void Chip8::SNE()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char NN = opcode & 0x00FF;
	if (V[X] != NN)
		movePC();

	movePC();
}

//5xy0 - SE Vx, Vy
//Skip next instruction if Vx = Vy.
void Chip8::SE2()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	if (V[X] == V[Y])
		movePC();

	movePC();
}

//6xnn - LD Vx, byte
//Set Vx = nn.
void Chip8::LD()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char NN = opcode & 0x00FF;
	V[X] = NN;
	movePC();
}

//7xnn - ADD Vx, byte
//Set Vx = Vx + nn.
void Chip8::ADD()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char NN = opcode & 0x00FF;
	V[X] += NN;
	movePC();
}

//8xy0 - LD Vx, Vy
//Set Vx = Vy.
void Chip8::LD2()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	V[X] = V[Y];
	movePC();
}

//8xy1 - OR Vx, Vy
//Set Vx = Vx OR Vy
void Chip8::OR()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	V[X] = V[X] | V[Y];
	movePC();
}

//8xy2 - AND Vx, Vy
//Set Vx = Vx AND Vy.
void Chip8::AND()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	V[X] = V[X] & V[Y];
	movePC();
}

//8xy3 - XOR Vx, Vy
//Set Vx = Vx XOR Vy.
void Chip8::XOR()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	V[X] = V[X] ^ V[Y];
	movePC();
}

//8xy4 - ADD Vx, Vy
//Set Vx = Vx + Vy, set VF = carry.
void Chip8::ADD2()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	// If the sum is larger than 255 set the carry flag for VF
	if (V[X] + V[Y] > 0xF0)
		V[0xF] = 1; // carry
	else
		V[0xF] = 0;

	V[X] += V[Y];

	movePC();
}

//8xy5 - SUB Vx, Vy
//Set Vx = Vx - Vy, set VF = NOT borrow.
void Chip8::SUB()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	// If the subtraction is below 0 set the borrow flag for VF
	if (V[X] > V[Y])
		V[0xF] = 1; // borrow
	else
		V[0xF] = 0;

	V[X] -= V[Y];

	movePC();
}

//8xy6 - SHR Vx {, Vy}
//Set Vx = Vx SHR 1.
void Chip8::SHR()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	//Get the least significant bit of VX and store it in VF to know whether the number is even or odd
	V[0xF] = V[X] % 0x2;

	//Shift VX right by 1
	V[X] = V[X] >> 0x1;

	movePC();
}

//8xy7 - SUBN Vx, Vy
//Set Vx = Vy - Vx, set VF = NOT borrow.
void Chip8::SUBN()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	if (V[Y] > V[X])
		V[0xF] = 1; // borrow
	else
		V[0xF] = 0;

	V[X] = V[Y] - V[X];

	movePC();
}


//8xyE - SHL Vx{ , Vy }
//Set Vx = Vx SHL 1.
void Chip8::SHL()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	//Get the least significant bit of VX and store it in VF to know whether the number is even or odd
	V[0xF] = V[X] % 0x2;

	//Shift VX left by 1
	V[X] = V[X] << 0x1;

	movePC();
}

//9xy0 - SNE Vx, Vy
//Skip next instruction if Vx != Vy.
void Chip8::SNE2()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	if (V[X] != V[Y])
		movePC();

	movePC();
}

//Annn - LD I, addr
//Set I = nnn.
void Chip8::LD3()
{
	// Get the trailing 12 bits 
	unsigned short NNN = opcode & 0x0FFF;

	// Store in the index register
	I = NNN;

	// Move the program counter by 2 bytes (short) to get the next instruction
	movePC();
}

//Bnnn - JP V0, addr
//Jump to location NNN + V0.
void Chip8::JP2()
{
	unsigned short NNN = opcode & 0x0FFF;
	pc = NNN;
}

//Cxnn - RND Vx, byte
//Set Vx = random byte AND nn.
void Chip8::RND()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char NN = opcode & 0x00FF;
	V[X] = (rand() % 0xFF) & NN;
	movePC();
}

//Dxyn - DRW Vx, Vy, nibble
//Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
void Chip8::DRW()
{
	unsigned short x = V[(opcode & 0x0F00) >> 8];
	unsigned short y = V[(opcode & 0x00F0) >> 4];
	unsigned short N = (opcode & 0x000F);//height
	const short WIDTH = 8; // 8 pixels

						  // 1000 0000 = 128
						  // 0100 0000 = 64
						  // 0010 0000 = 32
						  // 0001 0000 = 16
						  // 0000 1000 = 8
						  // 0000 0100 = 4
						  // 0000 0010 = 2
						  // 0000 0001 = 1

						  //bit is used to check if a spcific bit in the pixel byte is on or off
	unsigned short bit = 0x80;

	V[0xF] = 0;
	for (int i = 0; i < N; i++) // Height
	{
		unsigned short pixel = memory[I + i];
		for (int j = 0; j < WIDTH; j++) // Width
		{
			//Check if a pixel exists in memory, if so, set it in the gfx
			if ((pixel & (bit >> j)) != 0)
			{
				//Check for a collision
				if (gfx[y + i][x + j] == 1)
				{
					V[0xF] = 1;
				}

				gfx[y + i][x + j] ^= 1;
			}

		}
	}
	movePC();
}

//Ex9E - SKP Vx
//Skip next instruction if key with the value of Vx is pressed.
void Chip8::SKP()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	if (key[V[X]] != 0)
		movePC();
	movePC();
}

//ExA1 - SKNP Vx
//Skip next instruction if key with the value of Vx is not pressed.
void Chip8::SKNP()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	if (key[V[X]] == 0)
		movePC();
	movePC();
}

//Fx07 - LD Vx, DT
//Set Vx = delay timer value.
void Chip8::LD4()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	V[X] = delay_timer;
	movePC();
}

//Fx0A - LD Vx, K
//Wait for a key press, store the value of the key in Vx.
void Chip8::LD5()
{
	unsigned char X = (opcode & 0x0F00) >> 8;

	unsigned char currentKey = 0x0;

	const unsigned char MAX_KEY = 0xF;

	bool pressed = false;

	//check if at least one key is pressed
	for (int i = 0; i < MAX_KEY; i++)
	{
		if (key[i] != 0)
		{
			pressed = true;
			V[X] = key[i];
		}
	}

	//this will make this same opcode execute until a key is pressed
	if (!pressed)
		return;

	movePC();
}

//Fx15 - LD DT, Vx
//Set delay timer = Vx.
void Chip8::LD6()
{
	unsigned char X = (opcode & 0x0F00) >> 8;

	delay_timer = V[X];

	movePC();
}

//Fx18 - LD ST, Vx
//Set sound timer = Vx.
void Chip8::LD7()
{
	unsigned char X = (opcode & 0x0F00) >> 8;

	sound_timer = V[X];

	movePC();
}

//Fx1E - ADD I, Vx
//Set I = I + Vx.
void Chip8::ADD3()
{
	unsigned char X = (opcode & 0x0F00) >> 8;

	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
	if (I + V[X] > 0xFFF)
		V[0xF] = 1;
	else
		V[0xF] = 0;

	I += V[X];

	movePC();
}

//Fx29 - LD F, Vx
//Set I = location of sprite for digit Vx. Multiply by 5 because a sprite is 5 bytes
void Chip8::LD8()
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	I = V[X] * 0x5;
	movePC();
}

//Fx33 - LD B, Vx
//Store BCD representation of Vx in memory locations I, I + 1, and I + 2.
void Chip8::LD9()
{
	unsigned char X = (opcode & 0x0F00) >> 8;

	memory[I] = (V[X] / 100);
	memory[I + 1] = (V[X] % 100) / 10;
	memory[I + 2] = (V[X] % 10);

	movePC();
}


//Fx55 - LD[I], Vx
//Store registers V0 through Vx in memory starting at location I.
void Chip8::LD10()
{
	unsigned char X = (opcode & 0x0F00) >> 8;

	for (int i = 0; i < X; i++)
		memory[I + i] = V[i];
	// On the original interpreter, when the operation is done, I = I + X + 1.
	I += X + 1;
	movePC();
}

//Fx65 - LD Vx, [I]
//Read registers V0 through Vx from memory starting at location I.
void Chip8::LD11()
{
	unsigned char X = (opcode & 0x0F00) >> 8;

	for (int i = 0; i < X; i++)
		V[i] = memory[I + i];

	// On the original interpreter, when the operation is done, I = I + X + 1.
	I += X + 1;
	movePC();
}

//move the progarm counter by 2 bytes
void Chip8::movePC()
{
	pc += 2;
}

void Chip8::clearGFX()
{
	for (int i = 0; i < 32; ++i)
		for (int j = 0; j < 64; j++)
			gfx[i][j] = 0x00;
}
