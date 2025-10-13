//Стащил из Keil
#ifndef MACROS_H_
#define MACROS_H_

#define SET_BIT(REG, BIT)							((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)							((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)							((REG) & (BIT))
#define INVERSE_BIT(REG, BIT)						((REG) ^ (BIT))
#define CLEAR_REG(REG)								((REG) = (0x0))
#define WRITE_REG(REG, VAL)							((REG) = (VAL))
#define READ_REG(REG)								((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)			WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define ERROR										1
#define SUCCESS										0



#endif /* MACROS_H_ */