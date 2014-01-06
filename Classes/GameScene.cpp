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

	// バックキー・メニューキーイベントを取得する
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

    // タップしたコマのTag とコマの種類を取得
    int tag = 0;
    kBlock blockType;
    getTouchBlockTag(touchPoint, tag, blockType);

    if (tag != 0) {
        // 隣接するコマを検索する
        list<int> sameColorBlockTags = getSameColorBlockTags(tag, blockType);

        if (sameColorBlockTags.size() > 1) {
            // 得点加算 (消したコマ数 - 2) の2 乗
            m_score += pow(sameColorBlockTags.size() - 2, 2);

            // アニメーション開始
            m_animating = true;

            // 隣接するコマを削除する
            removeBlock(sameColorBlockTags, blockType);

            // コマ削除後のアニメーション
            movingBlocksAnimation1(sameColorBlockTags);
        }
    }
}

// 全コマに対して、隣り合うコマが存在するかチェック
bool GameScene::existsSameBlock() {
    // コマ種類のループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        // 各種類のコマ数分のループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            if (getSameColorBlockTags(*it2, *it1).size() > 1) {
                // 隣り合うコマが存在する場合は、trueを返す
                return true;
            }
            it2++;
        }
        it1++;
    }

    // 隣り合うコマが存在しない場合は、falseを返す
    return false;
}

// fetch position
map<int, bool> GameScene::getExistsBlockColumn() {
    // 検索配列初期化
    map<int, bool> xBlock;
    for (int i = 0; i < MAX_BLOCK_X; i++) {
        xBlock[i] = false;
    }

    // コマ種類のループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        // 各種類のコマ数分のループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            // 存在するコマのx 位置を記録
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
            // すでにリストにあるか検索
            if (!hasSameColorBlock(sameColorBlockTags, tags[i])) {
                // コマ配列にあるか検索
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

// リセットボタンタップ時の処理
void GameScene::menuResetCallback(cocos2d::CCObject* pSender)
{
    GameScene* scene = GameScene::create();
    CCDirector::sharedDirector()->replaceScene((CCScene*)scene);
}

// 配列のコマを削除
void GameScene::moveBlock() {
    // コマ種類のループ
    vector<kBlock>::iterator it1 = blockTypes.begin();
    while (it1 != blockTypes.end()) {
        // 各種類のコマ数分のループ
        list<int>::iterator it2 = m_blockTags[*it1].begin();
        while (it2 != m_blockTags[*it1].end()) {
            BlockSprite* blockSprite = (BlockSprite*)m_background->getChildByTag(*it2);
            int nextPosX = blockSprite->getNextPosX();
            int nextPosY = blockSprite->getNextPosY();

            if (nextPosX != -1 || nextPosY != -1) {
                // 新しいタグをセットする
                int newTag = getTag(nextPosX, nextPosY);
                blockSprite->initNextPos();
                blockSprite->setTag(newTag);

                // タグ一覧の値も新しいタグに変更する
                *it2 = newTag;

                // アニメーションをセットする
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
    // ラベル再表示
    showLabel();

    // アニメーション終了
    m_animating = false;

    // ゲーム終了チェック
    if (!existsSameBlock()) {
        // ハイスコア記録・表示
        saveHighScore();

        CCSize bgSize = m_background->getContentSize();

        // ゲーム終了表示
        CCSprite* gameOver = CCSprite::create(PNG_GAMEOVER);
        gameOver->setPosition(ccp(bgSize.width / 2, bgSize.height * 0.8));
        m_background->addChild(gameOver, kZOrderGameOver, kTagGameOver);

        setTouchEnabled(false);
    }
}

void GameScene::movingBlocksAnimation1(list<int> blocks) {
    // コマの新しい位置をセットする
    searchNewPosition1(blocks);

    // 新しい位置がセットされたコマのアニメーション
    moveBlock();

    // アニメーション終了時に次のアニメーション処理を開始する
    scheduleOnce(schedule_selector(GameScene::movingBlocksAnimation2), MOVING_TIME);
}

void GameScene::movingBlocksAnimation2() {
    // コマの新しい位置をセットする
    searchNewPosition2();

    // 新しい位置がセットされたコマのアニメーション
    moveBlock();

    // アニメーション終了時に次の処理を開始する
    scheduleOnce(schedule_selector(GameScene::movedBlocks), MOVING_TIME);
}

void GameScene::removeBlock(list<int> blockTags, kBlock blockType) {
    list<int>::iterator it = blockTags.begin();
    while (it != blockTags.end()) {
        // 既存配列から該当コマを削除
        m_blockTags[blockType].remove(*it);

        // 対象となるコマを取得
        CCNode* block = m_background->getChildByTag(*it);
        if (block) {
        	// コマが消えるアニメーションを生成
        	CCScaleTo* scale = CCScaleTo::create(REMOVING_TIME, 0.0f);

            // コマを削除するアクションを生成
            CCCallFuncN* func = CCCallFuncN::create(this, callfuncN_selector(GameScene::removingBlock));

            // アクションをつなげる
            CCFiniteTimeAction* action = CCSequence::create(scale, func, NULL);

            // アクションをセットする
            block->runAction(action);
        }

        it++;
    }
}

// コマの削除
void GameScene::removingBlock(CCNode* block) {
    block->removeFromParentAndCleanup(true);
}

// ハイスコア記録・表示
void GameScene::saveHighScore() {
    CCUserDefault* userDefault = CCUserDefault::sharedUserDefault();

    // ハイスコアを取得する
    int oldHighScore = userDefault->getIntegerForKey(KEY_HIGHSCORE, 0);
    if (oldHighScore < m_score) {
        // ハイスコアを保持する
        userDefault->setIntegerForKey(KEY_HIGHSCORE, m_score);
        userDefault->flush();

        // ハイスコアを表示する
        showHighScoreLabel();
    }
}

// ハイスコアラベル表示
void GameScene::showHighScoreLabel() {
    CCSize bgSize = m_background->getContentSize();

    // ハイスコア表示
    int highScore = CCUserDefault::sharedUserDefault()->getIntegerForKey(KEY_HIGHSCORE, 0);
    const char* highScoreStr = ccsf("%d", highScore);
    CCLabelBMFont* highScoreLabel = (CCLabelBMFont*)m_background->getChildByTag(kTagHighScoreLabel);
    if (!highScoreLabel) {
        // ハイスコア生成
        highScoreLabel = CCLabelBMFont::create(highScoreStr, FONT_WHITE);
        highScoreLabel->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.87));
        m_background->addChild(highScoreLabel, kZOrderLabel, kTagHighScoreLabel);
    } else {
        highScoreLabel->setString(highScoreStr);
    }
}

// リセットボタン作成
void GameScene::showResetButton()
{
    CCSize bgSize = m_background->getContentSize();

    // リセットボタン作成
    CCMenuItemImage* resetButton = CCMenuItemImage::create(PNG_RESET, PNG_RESET, this, menu_selector(GameScene::menuResetCallback));
    resetButton->setPosition(ccp(bgSize.width * 0.78, bgSize.height * 0.1));

    // メニュー作成
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
    // 消えるコマ数分のループ
    list<int>::iterator it1 = blocks.begin();
    while (it1 != blocks.end()) {
        PositionIndex posIndex1 = getPositionIndex(*it1);

        // コマ種類のループ
        vector<kBlock>::iterator it2 = blockTypes.begin();
        while (it2 != blockTypes.end()) {
            // 各種類のコマ数分のループ
            list<int>::iterator it3 = m_blockTags[*it2].begin();
            while (it3 != m_blockTags[*it2].end()) {
                PositionIndex posIndex2 = getPositionIndex(*it3);

                if (posIndex1.x == posIndex2.x && posIndex1.y < posIndex2.y) {
                    // 消えるコマの上に位置するコマに対して、移動先の位置をセットする
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
// コマ削除後のアニメーション
// コマの移動完了
// 新しい位置をセット
void GameScene::searchNewPosition2()
{
    // 存在する列を取得する
    map<int, bool> xBlock = getExistsBlockColumn();

    // コマが存在しないx 位置を埋める
    bool first = true;
    for (int i = MAX_BLOCK_X - 1; i >= 0; i--) {
        if (xBlock[i]) {
            // コマが存在する
            first = false;
            continue;
        } else {
            // コマが存在しない
            if (first) {
                // 右側にコマがない
                continue;
            } else {
                // この位置より右側にあるコマを左に1 つ寄せる

                // コマ種類のループ
                vector<kBlock>::iterator it1 = blockTypes.begin();
                while (it1 != blockTypes.end()) {
                    // 各種類のコマ数分のループ
                    list<int>::iterator it2 = m_blockTags[*it1].begin();
                    while (it2 != m_blockTags[*it1].end()) {
                        PositionIndex posIndex = getPositionIndex(*it2);

                        if (i < posIndex.x) {
                            // 移動先の位置をセットする
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

    // 移動先の位置をセット
    blockSprite->setNextPos(--nextPosX, posIndex.y);
}

// 存在する列を取得する
// 消えたコマ列を埋めるように新しい位置をセット
// コマのアニメーション
// ラベル表示
void GameScene::showLabel() {
    CCSize bgSize = m_background->getContentSize();

    // 残数表示
    int tagsForLabel[] = {kTagRedLabel, kTagBlueLabel, kTagYellowLabel, kTagGreenLabel, kTagGrayLabel};
    const char* fontNames[] = {FONT_RED, FONT_BLUE, FONT_YELLOW, FONT_GREEN, FONT_GRAY};
    float heightRate[] = {0.61, 0.51, 0.41, 0.31, 0.21};

    // コマ種類のループ
    vector<kBlock>::iterator it = blockTypes.begin();
    while (it != blockTypes.end()) {
        // コマ残数表示
        int count = m_blockTags[*it].size();
        const char* countStr = ccsf("%02d", count);
        CCLabelBMFont* label = (CCLabelBMFont*)m_background->getChildByTag(tagsForLabel[*it]);
        if (!label) {
            // コマ残数生成
            label = CCLabelBMFont::create(countStr, fontNames[*it]);
            label->setPosition(ccp(bgSize.width * 0.78, bgSize.height * heightRate[*it]));
            m_background->addChild(label, kZOrderLabel, tagsForLabel[*it]);
        } else {
            label->setString(countStr);
        }
        it++;
    }

    // スコア表示
    const char* scoreStr = ccsf("%d", m_score);
    CCLabelBMFont* scoreLabel = (CCLabelBMFont*)m_background->getChildByTag(kTagScoreLabel);
    if (!scoreLabel) {
        // スコア生成
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

// Androidバックキーイベント
void GameScene::keyBackClicked()
{
    CCDirector::sharedDirector()->end();
}

// Androidメニューキーイベント
void GameScene::keyMenuClicked()
{
    menuResetCallback(NULL);
}

