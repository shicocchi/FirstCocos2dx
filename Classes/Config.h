/*
 * Config.h
 *
 *  Created on: 2014/01/06
 *      Author: shicocchi
 */

#ifndef CONFIG_H_
#define CONFIG_H_

enum kBlock {
	kBlockRed,
	kBlockBlue,
	kBlockYellow,
	kBlockGreen,
	kBlockGray,
	kBlockCount,
};

#define ccsf(...) CCString::createWithFormat(__VA_ARGS__)->getCString()

#endif /* CONFIG_H_ */
