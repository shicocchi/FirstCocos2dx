/*
 * GameScene.cpp
 *
 *  Created on: 2014/01/06
 *      Author: shicocchi
 */

#include "GameScene.h"
#include "BlockSprite.h"

using namespace cocos2d;
using namespace std;

// initialization variables
bool GameScene::init() {
	if(!CCLayer::init()) {
		return false;
	}

	// fetch touch event
	setTouchEnabled(true);
	setTouchMode(kCCTouchesOneByOne);

	// �o�b�N�L�[�E���j���[�L�[�C�x���g���擾����
	setKeypadEnabled(true);

	// initialize variables
	initForVariables();

	// show Background
	showBackground();

	// show blocks
	showBlock();

	// show labels
	showLabel();
	showHighScoreLabel();

	// show reset button
	showResetButton();

	return true;
}

CCScene* GameScene::scene() {
	CCScene* scene = CCScene::create();
	GameScene* layer = GameScene::create();
	scene->addChild(layer);
	return scene;
}

bool GameScene::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent) {
	// duaring animation return false
	return !m_animating;
}

void GameScene::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent) {
	// fetch touched point
    CCPoint touchPoint = m_background->convertTouchToNodeSpace(pTouch);

    // �^�b�v�����R�}��Tag �ƃR�}�̎�ނ��擾
    int tag = 0;
    kBlock blockType;
    getTouchBlockTag(touchPoint, tag, blockType);

    if (tag != 0) {
        // �אڂ���R�}����������
        list<int> sameColorBlockTags = getSameColorBlockTags(tag, blockType);

        if (sameColorBlockTags.size() > 1) {
            // ���_���Z (�������R�}�� - 2) ��2 ��
            m_score += pow(sameColorBlockTags.size() - 2, 2);

            // �A�j���[�V�����J�n
            m_animating = true;

            // �אڂ���R�}���폜����
            removeBlock(sameColorBlockTags, blockType);

            // �R�}�폜��̃A�j���[�V����
            movingBlocksAnimation1(sameColorBlockTags);
        }
    }
}

// �S�R�}�ɑ΂��āA�ׂ荇���R�}�����݂��邩�`�F�b�N
bool GameScene::existsSameBlock() {
    // �R�}��ނ̃��[�v
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        // �e��ނ̃R�}�����̃��[�v
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            if (getSameColorBlockTags(*it2, *it1).size() > 1) {
                // �ׂ荇���R�}�����݂���ꍇ�́Atrue��Ԃ�
                return true;
            }
            it2++;
        }
        it1++;
    }

    // �ׂ荇���R�}�����݂��Ȃ��ꍇ�́Afalse��Ԃ�
    return false;
}

// fetch position
map<int, bool> GameScene::getExistsBlockColumn() {
    // �����z�񏉊���
    map<int, bool> xBlock;
    for (int i = 0; i < MAX_BLOCK_X; i++) {
        xBlock[i] = false;
    }

    // �R�}��ނ̃��[�v
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        // �e��ނ̃R�}�����̃��[�v
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            // ���݂���R�}��x �ʒu���L�^
            xBlock[getPositionIndex(*it2).x] = true;
            it2++;
        }
        it1++;
    }

    return xBlock;
}

// fetch tag
// show blocks
list<int> GameScene::getSameColorBlockTags(int baseTag, kBlock blockType) {
    // initialize same color tag list
    list<int> sameColorBlockTags;
    sameColorBlockTags.push_back(baseTag);

    list<int>::iterator it = sameColorBlockTags.begin();
    while (it != sameColorBlockTags.end()) {
        int tags[] = {
            *it + 100, // right block tag
            *it - 100, // left block tag
            *it + 1, // up block tag
            *it - 1, // down block tag
        };

        for (int i = 0; i < sizeof(tags) / sizeof(tags[0]); i++) {
            // ���łɃ��X�g�ɂ��邩����
            if (!hasSameColorBlock(sameColorBlockTags, tags[i])) {
                // �R�}�z��ɂ��邩����
                if (hasSameColorBlock(m_blockTags[blockType], tags[i])) {
                    sameColorBlockTags.push_back(tags[i]);
                }
            }
        }
        it++;
    }

    return sameColorBlockTags;
}

// touch began event
// touch ended event
// fetch the touched block tag
int GameScene::getTag(int posIndexX, int posIndexY) {
	return kTagBaseBlock + posIndexX * 100 + posIndexY;
}

void GameScene::getTouchBlockTag(CCPoint touchPoint, int &tag, kBlock &blockType) {
    for (int x = 0; x < MAX_BLOCK_X; x++) {
        for (int y = 0; y < MAX_BLOCK_Y; y++) {
            int currentTag = getTag(x, y);
            CCNode* node = m_background->getChildByTag(currentTag);
            if (node && node->boundingBox().containsPoint(touchPoint)) {
                tag = currentTag;
                blockType = ((BlockSprite*)node)->getBlockType();

                return;
            }
        }
    }
}

// search contain same tag
CCPoint GameScene::getPosition(int posIndexX, int posIndexY) {
	float offsetX = m_background->getContentSize().width * 0.168;
	float offsetY = m_background->getContentSize().height * 0.029;
	return CCPoint((posIndexX + 0.5) * m_blockSize + offsetX, (posIndexY + 0.5) * m_blockSize + offsetY);
}

GameScene::PositionIndex GameScene::getPositionIndex(int tag) {
    int pos1_x = (tag - kTagBaseBlock) / 100;
    int pos1_y = (tag - kTagBaseBlock) % 100;

    return PositionIndex(pos1_x, pos1_y);
}

bool GameScene::hasSameColorBlock(list<int> blockTagList, int searchBlockTag) {
    list<int>::iterator it;
    for (it = blockTagList.begin(); it != blockTagList.end(); ++it) {
        if (*it == searchBlockTag) {
            return true;
        }
    }

    return false;
}

// fetch same color ** list
void GameScene::initForVariables() {
	// initialize random number
	srand((unsigned)time(NULL));

	// fetch the length of block
	BlockSprite* pBlock = BlockSprite::createWithBlockType(kBlockRed);
	m_blockSize = pBlock->getContentSize().height;

	// create block list
	blockTypes.push_back(kBlockRed);
	blockTypes.push_back(kBlockBlue);
	blockTypes.push_back(kBlockYellow);
	blockTypes.push_back(kBlockGreen);
	blockTypes.push_back(kBlockGray);

	// initialize variable
	m_animating = false;
	m_score = 0;
}

// ���Z�b�g�{�^���^�b�v���̏���
void GameScene::menuResetCallback(cocos2d::CCObject* pSender)
{
    GameScene* scene = GameScene::create();
    CCDirector::sharedDirector()->replaceScene((CCScene*)scene);
}

// �z��̃R�}���폜
void GameScene::moveBlock() {
    // �R�}��ނ̃��[�v
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        // �e��ނ̃R�}�����̃��[�v
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(*it2);
            int nextPosX = blockSprite->getNextPosX();
            int nextPosY = blockSprite->getNextPosY();

            if (nextPosX != -1 || nextPosY != -1) {
                // �V�����^�O���Z�b�g����
                int newTag = getTag(nextPosX, nextPosY);
                blockSprite->initNextPos();
                blockSprite->setTag(newTag);

                // �^�O�ꗗ�̒l���V�����^�O�ɕύX����
                *it2 = newTag;

                // �A�j���[�V�������Z�b�g����
                CCMoveTo* move = CCMoveTo::create(MOVING_TIME, getPosition(nextPosX, nextPosY));
                blockSprite->runAction(move);
            }
            it2++;
        }
        it1++;
    }
}

void GameScene::movedBlocks()
{
    // ���x���ĕ\��
    showLabel();

    // �A�j���[�V�����I��
    m_animating = false;

    // �Q�[���I���`�F�b�N
    if (!existsSameBlock()) {
        // �n�C�X�R�A�L�^�E�\��
        saveHighScore();

        CCSize bgSize = m_background->getContentSize();

        // �Q�[���I���\��
        CCSprite* gameOver = CCSprite::create(PNG_GAMEOVER);
        gameOver->setPosition(ccp(bgSize.width / 2, bgSize.height * 0.8));
        m_background->addChild(gameOver, kZOrderGameOver, kTagGameOver);

        setTouchEnabled(false);
    }
}

void GameScene::movingBlocksAnimation1(list<int> blocks) {
    // �R�}�̐V�����ʒu���Z�b�g����
    searchNewPosition1(blocks);

    // �V�����ʒu���Z�b�g���ꂽ�R�}�̃A�j���[�V����
    moveBlock();

    // �A�j���[�V�����I�����Ɏ��̃A�j���[�V�����������J�n����
    scheduleOnce(schedule_selector(GameScene::movingBlocksAnimation2), MOVING_TIME);
}

void GameScene::movingBlocksAnimation2() {
    // �R�}�̐V�����ʒu���Z�b�g����
    searchNewPosition2();

    // �V�����ʒu���Z�b�g���ꂽ�R�}�̃A�j���[�V����
    moveBlock();

    // �A�j���[�V�����I�����Ɏ��̏������J�n����
    scheduleOnce(schedule_selector(GameScene::movedBlocks), MOVING_TIME);
}

void GameScene::removeBlock(list<int> blockTags, kBlock blockType) {
    list<int>::iterator it = blockTags.begin();
    while (it != blockTags.end()) {
        // �����z�񂩂�Y���R�}���폜
        m_blockTags[blockType].remove(*it);

        // �ΏۂƂȂ�R�}���擾
        CCNode* block = m_background->getChildByTag(*it);
        if (block) {
        	// �R�}��������A�j���[�V�����𐶐�
        	CCScaleTo* scale = CCScaleTo::create(REMOVING_TIME, 0.0f);

            // �R�}���폜����A�N�V�����𐶐�
            CCCallFuncN* func = CCCallFuncN::create(this, callfuncN_selector(GameScene::removingBlock));

            // �A�N�V�������Ȃ���
            CCFiniteTimeAction* action = CCSequence::create(scale, func, NULL);

            // �A�N�V�������Z�b�g����
            block->runAction(action);
        }

        it++;
    }
}

// �R�}�̍폜
void GameScene::removingBlock(CCNode* block) {
    block->removeFromParentAndCleanup(true);
}

// �n�C�X�R�A�L�^�E�\��
void GameScene::saveHighScore() {
    CCUserDefault* userDefault = CCUserDefault::sharedUserDefault();

    // �n�C�X�R�A���擾����
    int oldHighScore = userDefault->getIntegerForKey(KEY_HIGHSCORE, 0);
    if (oldHighScore < m_score) {
        // �n�C�X�R�A��ێ�����
        userDefault->setIntegerForKey(KEY_HIGHSCORE, m_score);
        userDefault->flush();

        // �n�C�X�R�A��\������
        showHighScoreLabel();
    }
}

// �n�C�X�R�A���x���\��
void GameScene::showHighScoreLabel() {
    CCSize bgSize = m_background->getContentSize();

    // �n�C�X�R�A�\��
    int highScore = CCUserDefault::sharedUserDefault()->getIntegerForKey(KEY_HIGHSCORE, 0);
    const char* highScoreStr = ccsf("%d", highScore);
    CCLabelBMFont* highScoreLabel = (CCLabelBMFont*)m_background->getChildByTag(kTagHighScoreLabel);
    if (!highScoreLabel) {
        // �n�C�X�R�A����
        highScoreLabel = CCLabelBMFont::create(highScoreStr, FONT_WHITE);
        highScoreLabel->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.87));
        m_background->addChild(highScoreLabel, kZOrderLabel, kTagHighScoreLabel);
    } else {
        highScoreLabel->setString(highScoreStr);
    }
}

// ���Z�b�g�{�^���쐬
void GameScene::showResetButton()
{
    CCSize bgSize = m_background->getContentSize();

    // ���Z�b�g�{�^���쐬
    CCMenuItemImage* resetButton = CCMenuItemImage::create(PNG_RESET, PNG_RESET, this, menu_selector(GameScene::menuResetCallback));
    resetButton->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.1));

    // ���j���[�쐬
    CCMenu* menu = CCMenu::create(resetButton, NULL);
    menu->setPosition(CCPointZero);
    m_background->addChild(menu);
}

void GameScene::setNewPosition1(int tag, PositionIndex posIndex) {
    BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
    int nextPosY = blockSprite->getNextPosY();
    if (nextPosY == -1) {
        nextPosY = posIndex.y;
    }

    // set next position
    blockSprite->setNextPos(posIndex.x, --nextPosY);
}

void GameScene::searchNewPosition1(list<int> blocks) {
    // ������R�}�����̃��[�v
    list<int>::iterator it1 = blocks.begin();
    while (it1 != blocks.end()) {
        PositionIndex posIndex1 = getPositionIndex(*it1);

        // �R�}��ނ̃��[�v
        vector<kBlock>::iterator it2 = blockTypes.begin();
        while (it2 != blockTypes.end()) {
            // �e��ނ̃R�}�����̃��[�v
            list<int>::iterator it3 = m_blockTags[*it2].begin();
            while (it3 != m_blockTags[*it2].end()) {
                PositionIndex posIndex2 = getPositionIndex(*it3);

                if (posIndex1.x == posIndex2.x && posIndex1.y < posIndex2.y) {
                    // ������R�}�̏�Ɉʒu����R�}�ɑ΂��āA�ړ���̈ʒu���Z�b�g����
                    setNewPosition1(*it3, posIndex2);
                }
                it3++;
            }
            it2++;
        }
        it1++;
    }
}

// move blocks
// �R�}�폜��̃A�j���[�V����
// �R�}�̈ړ�����
// �V�����ʒu���Z�b�g
void GameScene::searchNewPosition2()
{
    // ���݂������擾����
    map<int, bool> xBlock = getExistsBlockColumn();

    // �R�}�����݂��Ȃ�x �ʒu�𖄂߂�
    bool first = true;
    for (int i = MAX_BLOCK_X - 1; i >= 0; i--) {
        if (xBlock[i]) {
            // �R�}�����݂���
            first = false;
            continue;
        } else {
            // �R�}�����݂��Ȃ�
            if (first) {
                // �E���ɃR�}���Ȃ�
                continue;
            } else {
                // ���̈ʒu���E���ɂ���R�}������1 �񂹂�

                // �R�}��ނ̃��[�v
                vector<kBlock>::iterator it1 = blockTypes.begin();
                while (it1 != blockTypes.end()) {
                    // �e��ނ̃R�}�����̃��[�v
                    list<int>::iterator it2 = m_blockTags[*it1].begin();
                    while (it2 != m_blockTags[*it1].end()) {
                        PositionIndex posIndex = getPositionIndex(*it2);

                        if (i < posIndex.x) {
                            // �ړ���̈ʒu���Z�b�g����
                            setNewPosition2(*it2, posIndex);
                        }
                        it2++;
                    }
                    it1++;
                }
            }
        }
    }
}

void GameScene::setNewPosition2(int tag, PositionIndex posIndex) {
    BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(tag);
    int nextPosX = blockSprite->getNextPosX();
    if (nextPosX == -1) {
        nextPosX = posIndex.x;
    }

    // �ړ���̈ʒu���Z�b�g
    blockSprite->setNextPos(--nextPosX, posIndex.y);
}

// ���݂������擾����
// �������R�}��𖄂߂�悤�ɐV�����ʒu���Z�b�g
// �R�}�̃A�j���[�V����
// ���x���\��
void GameScene::showLabel() {
    CCSize bgSize = m_background->getContentSize();

    // �c���\��
    int tagsForLabel[] = {kTagRedLabel, kTagBlueLabel, kTagYellowLabel, kTagGreenLabel, kTagGrayLabel};
    const char* fontNames[] = {FONT_RED, FONT_BLUE, FONT_YELLOW, FONT_GREEN, FONT_GRAY};
    float heightRate[] = {0.61, 0.51, 0.41, 0.31, 0.21};

    // �R�}��ނ̃��[�v
    vector<kBlock>::iterator it = blockTypes.begin();
    while (it != blockTypes.end()) {
        // �R�}�c���\��
        int count = m_blockTags[*it].size();
        const char* countStr = ccsf("%02d", count);
        CCLabelBMFont* label = (CCLabelBMFont*)m_background->getChildByTag(tagsForLabel[*it]);
        if (!label) {
            // �R�}�c������
            label = CCLabelBMFont::create(countStr, fontNames[*it]);
            label->setPosition(ccp(bgSize.width * 0.78, bgSize.height * heightRate[*it]));
            m_background->addChild(label, kZOrderLabel, tagsForLabel[*it]);
        } else {
            label->setString(countStr);
        }
        it++;
    }

    // �X�R�A�\��
    const char* scoreStr = ccsf("%d", m_score);
    CCLabelBMFont* scoreLabel = (CCLabelBMFont*)m_background->getChildByTag(kTagScoreLabel);
    if (!scoreLabel) {
        // �X�R�A����
        scoreLabel = CCLabelBMFont::create(scoreStr, FONT_WHITE);
        scoreLabel->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.75));
        m_background->addChild(scoreLabel, kZOrderLabel, kTagScoreLabel);
    } else {
        scoreLabel->setString(scoreStr);
    }
}

// showBackground
void GameScene::showBackground() {
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	// create background
	m_background = CCSprite::create(PNG_BACKGROUND);
	m_background->setPosition(ccp(winSize.width / 2, winSize.height / 2));
	addChild(m_background, kZOrderBackground, kTagBackground);
}

void GameScene::showBlock() {
	// create 8x8 blocks
	for(int x = 0; x < MAX_BLOCK_X; x++) {
		for(int y = 0; y < MAX_BLOCK_Y; y++) {
			// create random block
			kBlock blockType = (kBlock)(rand() % kBlockCount);

			// add tag
			int tag = getTag(x, y);
			m_blockTags[blockType].push_back(tag);

			// create block
			BlockSprite* pBlock = BlockSprite::createWithBlockType(blockType);
			pBlock->setPosition(getPosition(x, y));
			m_background->addChild(pBlock, kZOrderBlock, tag);
		}
	}
}

// Android�o�b�N�L�[�C�x���g
void GameScene::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}

// Android���j���[�L�[�C�x���g
void GameScene::keyMenuClicked()
{
    menuResetCallback(NULL);
}

