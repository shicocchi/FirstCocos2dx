/*
 * BlockSprite.h
 *
 *  Created on: 2014/01/06
 *      Author: shicocchi
 */

#ifndef BLOCKSPRITE_H_
#define BLOCKSPRITE_H_

#include "cocos2d.h"
#include "Config.h"

class BlockSprite : public cocos2d::CCSprite {
	protected:
		const char* getBlockImageFileName(kBlock blockType);

	public:
		CC_SYNTHESIZE_READONLY(kBlock, m_blockType, BlockType);
		CC_SYNTHESIZE_READONLY(int, m_nextPosX, NextPosX);
		CC_SYNTHESIZE_READONLY(int, m_nextPosY, NextPosY);

		void initNextPos();
		void setNextPos(int nextPostX, int nextPostY);
		BlockSprite();
		virtual ~BlockSprite();
		virtual bool initWithBlockType(kBlock blockType);
		static BlockSprite* createWithBlockType(kBlock blockType);
};



#endif /* BLOCKSPRITE_H_ */
