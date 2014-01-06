/*
 * BlockSprite.cpp
 *
 *  Created on: 2014/01/06
 *      Author: shicocchi
 */

#include "BlockSprite.h"

BlockSprite::BlockSprite() {
	initNextPos();
}

BlockSprite::~BlockSprite() {
}

// initialize next position
void BlockSprite::initNextPos() {
	m_nextPosX = -1;
	m_nextPosY = -1;
}

// set next position
void BlockSprite::setNextPos(int nextPosX, int nextPosY) {
	m_nextPosX = nextPosX;
	m_nextPosY = nextPosY;
}

BlockSprite* BlockSprite::createWithBlockType(kBlock blockType) {
	BlockSprite *pRet = new BlockSprite();
	if(pRet && pRet->initWithBlockType(blockType)) {
		pRet->autorelease();
		return pRet;
	} else {
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

bool BlockSprite::initWithBlockType(kBlock blockType) {
	if(!CCSprite::initWithFile(getBlockImageFileName(blockType))) {
		return false;
	}

	m_blockType = blockType;

	return true;
}

const char* BlockSprite::getBlockImageFileName(kBlock blockType) {
	switch(blockType) {
		case kBlockRed:
			return "red.png";
		case kBlockBlue:
			return "blue.png";
		case kBlockYellow:
			return "yellow.png";
		case kBlockGreen:
			return "green.png";
		case kBlockGray:
			return "gray.png";
		default:
			CCAssert(false, "invalid blockType");
			return "";
	}
}

