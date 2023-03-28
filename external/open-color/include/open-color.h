#pragma once

// open_color.h provides minimalistic RGB color definition from Open color
// license: MIT
//
// https://github.com/yeun/open-color/blob/master/open-color.js
// conveted from https://github.com/yeun/open-color/blob/master/open-color.js

#include <array>
#include <cstdint>

namespace open_color
{
    using Hex = std::uint32_t;
    using Hexs = std::array<Hex, 10>;

    constexpr Hex white = 0xffffff;

    constexpr Hex black = 0x000000;

    constexpr Hex gray_0 = 0xf8f9fa;
    constexpr Hex gray_1 = 0xf1f3f5;
    constexpr Hex gray_2 = 0xe9ecef;
    constexpr Hex gray_3 = 0xdee2e6;
    constexpr Hex gray_4 = 0xced4da;
    constexpr Hex gray_5 = 0xadb5bd;
    constexpr Hex gray_6 = 0x868e96;
    constexpr Hex gray_7 = 0x495057;
    constexpr Hex gray_8 = 0x343a40;
    constexpr Hex gray_9 = 0x212529;
    constexpr Hexs gray = {gray_0, gray_1, gray_2, gray_3, gray_4, gray_5, gray_6, gray_7, gray_8, gray_9};

    constexpr Hex red_0 = 0xfff5f5;
    constexpr Hex red_1 = 0xffe3e3;
    constexpr Hex red_2 = 0xffc9c9;
    constexpr Hex red_3 = 0xffa8a8;
    constexpr Hex red_4 = 0xff8787;
    constexpr Hex red_5 = 0xff6b6b;
    constexpr Hex red_6 = 0xfa5252;
    constexpr Hex red_7 = 0xf03e3e;
    constexpr Hex red_8 = 0xe03131;
    constexpr Hex red_9 = 0xc92a2a;
    constexpr Hexs red = {red_0, red_1, red_2, red_3, red_4, red_5, red_6, red_7, red_8, red_9};

    constexpr Hex pink_0 = 0xfff0f6;
    constexpr Hex pink_1 = 0xffdeeb;
    constexpr Hex pink_2 = 0xfcc2d7;
    constexpr Hex pink_3 = 0xfaa2c1;
    constexpr Hex pink_4 = 0xf783ac;
    constexpr Hex pink_5 = 0xf06595;
    constexpr Hex pink_6 = 0xe64980;
    constexpr Hex pink_7 = 0xd6336c;
    constexpr Hex pink_8 = 0xc2255c;
    constexpr Hex pink_9 = 0xa61e4d;
    constexpr Hexs pink = {pink_0, pink_1, pink_2, pink_3, pink_4, pink_5, pink_6, pink_7, pink_8, pink_9};

    constexpr Hex grape_0 = 0xf8f0fc;
    constexpr Hex grape_1 = 0xf3d9fa;
    constexpr Hex grape_2 = 0xeebefa;
    constexpr Hex grape_3 = 0xe599f7;
    constexpr Hex grape_4 = 0xda77f2;
    constexpr Hex grape_5 = 0xcc5de8;
    constexpr Hex grape_6 = 0xbe4bdb;
    constexpr Hex grape_7 = 0xae3ec9;
    constexpr Hex grape_8 = 0x9c36b5;
    constexpr Hex grape_9 = 0x862e9c;
    constexpr Hexs grape = {grape_0, grape_1, grape_2, grape_3, grape_4, grape_5, grape_6, grape_7, grape_8, grape_9};

    constexpr Hex violet_0 = 0xf3f0ff;
    constexpr Hex violet_1 = 0xe5dbff;
    constexpr Hex violet_2 = 0xd0bfff;
    constexpr Hex violet_3 = 0xb197fc;
    constexpr Hex violet_4 = 0x9775fa;
    constexpr Hex violet_5 = 0x845ef7;
    constexpr Hex violet_6 = 0x7950f2;
    constexpr Hex violet_7 = 0x7048e8;
    constexpr Hex violet_8 = 0x6741d9;
    constexpr Hex violet_9 = 0x5f3dc4;
    constexpr Hexs violet = {violet_0, violet_1, violet_2, violet_3, violet_4, violet_5, violet_6, violet_7, violet_8, violet_9};

    constexpr Hex indigo_0 = 0xedf2ff;
    constexpr Hex indigo_1 = 0xdbe4ff;
    constexpr Hex indigo_2 = 0xbac8ff;
    constexpr Hex indigo_3 = 0x91a7ff;
    constexpr Hex indigo_4 = 0x748ffc;
    constexpr Hex indigo_5 = 0x5c7cfa;
    constexpr Hex indigo_6 = 0x4c6ef5;
    constexpr Hex indigo_7 = 0x4263eb;
    constexpr Hex indigo_8 = 0x3b5bdb;
    constexpr Hex indigo_9 = 0x364fc7;
    constexpr Hexs indigo = {indigo_0, indigo_1, indigo_2, indigo_3, indigo_4, indigo_5, indigo_6, indigo_7, indigo_8, indigo_9};

    constexpr Hex blue_0 = 0xe7f5ff;
    constexpr Hex blue_1 = 0xd0ebff;
    constexpr Hex blue_2 = 0xa5d8ff;
    constexpr Hex blue_3 = 0x74c0fc;
    constexpr Hex blue_4 = 0x4dabf7;
    constexpr Hex blue_5 = 0x339af0;
    constexpr Hex blue_6 = 0x228be6;
    constexpr Hex blue_7 = 0x1c7ed6;
    constexpr Hex blue_8 = 0x1971c2;
    constexpr Hex blue_9 = 0x1864ab;
    constexpr Hexs blue = {blue_0, blue_1, blue_2, blue_3, blue_4, blue_5, blue_6, blue_7, blue_8, blue_9};

    constexpr Hex cyan_0 = 0xe3fafc;
    constexpr Hex cyan_1 = 0xc5f6fa;
    constexpr Hex cyan_2 = 0x99e9f2;
    constexpr Hex cyan_3 = 0x66d9e8;
    constexpr Hex cyan_4 = 0x3bc9db;
    constexpr Hex cyan_5 = 0x22b8cf;
    constexpr Hex cyan_6 = 0x15aabf;
    constexpr Hex cyan_7 = 0x1098ad;
    constexpr Hex cyan_8 = 0x0c8599;
    constexpr Hex cyan_9 = 0x0b7285;
    constexpr Hexs cyan = {cyan_0, cyan_1, cyan_2, cyan_3, cyan_4, cyan_5, cyan_6, cyan_7, cyan_8, cyan_9};

    constexpr Hex teal_0 = 0xe6fcf5;
    constexpr Hex teal_1 = 0xc3fae8;
    constexpr Hex teal_2 = 0x96f2d7;
    constexpr Hex teal_3 = 0x63e6be;
    constexpr Hex teal_4 = 0x38d9a9;
    constexpr Hex teal_5 = 0x20c997;
    constexpr Hex teal_6 = 0x12b886;
    constexpr Hex teal_7 = 0x0ca678;
    constexpr Hex teal_8 = 0x099268;
    constexpr Hex teal_9 = 0x087f5b;
    constexpr Hexs teal = {teal_0, teal_1, teal_2, teal_3, teal_4, teal_5, teal_6, teal_7, teal_8, teal_9};

    constexpr Hex green_0 = 0xebfbee;
    constexpr Hex green_1 = 0xd3f9d8;
    constexpr Hex green_2 = 0xb2f2bb;
    constexpr Hex green_3 = 0x8ce99a;
    constexpr Hex green_4 = 0x69db7c;
    constexpr Hex green_5 = 0x51cf66;
    constexpr Hex green_6 = 0x40c057;
    constexpr Hex green_7 = 0x37b24d;
    constexpr Hex green_8 = 0x2f9e44;
    constexpr Hex green_9 = 0x2b8a3e;
    constexpr Hexs green = {green_0, green_1, green_2, green_3, green_4, green_5, green_6, green_7, green_8, green_9};

    constexpr Hex lime_0 = 0xf4fce3;
    constexpr Hex lime_1 = 0xe9fac8;
    constexpr Hex lime_2 = 0xd8f5a2;
    constexpr Hex lime_3 = 0xc0eb75;
    constexpr Hex lime_4 = 0xa9e34b;
    constexpr Hex lime_5 = 0x94d82d;
    constexpr Hex lime_6 = 0x82c91e;
    constexpr Hex lime_7 = 0x74b816;
    constexpr Hex lime_8 = 0x66a80f;
    constexpr Hex lime_9 = 0x5c940d;
    constexpr Hexs lime = {lime_0, lime_1, lime_2, lime_3, lime_4, lime_5, lime_6, lime_7, lime_8, lime_9};

    constexpr Hex yellow_0 = 0xfff9db;
    constexpr Hex yellow_1 = 0xfff3bf;
    constexpr Hex yellow_2 = 0xffec99;
    constexpr Hex yellow_3 = 0xffe066;
    constexpr Hex yellow_4 = 0xffd43b;
    constexpr Hex yellow_5 = 0xfcc419;
    constexpr Hex yellow_6 = 0xfab005;
    constexpr Hex yellow_7 = 0xf59f00;
    constexpr Hex yellow_8 = 0xf08c00;
    constexpr Hex yellow_9 = 0xe67700;
    constexpr Hexs yellow = {yellow_0, yellow_1, yellow_2, yellow_3, yellow_4, yellow_5, yellow_6, yellow_7, yellow_8, yellow_9};

    constexpr Hex orange_0 = 0xfff4e6;
    constexpr Hex orange_1 = 0xffe8cc;
    constexpr Hex orange_2 = 0xffd8a8;
    constexpr Hex orange_3 = 0xffc078;
    constexpr Hex orange_4 = 0xffa94d;
    constexpr Hex orange_5 = 0xff922b;
    constexpr Hex orange_6 = 0xfd7e14;
    constexpr Hex orange_7 = 0xf76707;
    constexpr Hex orange_8 = 0xe8590c;
    constexpr Hex orange_9 = 0xd9480f;
    constexpr Hexs orange = {orange_0, orange_1, orange_2, orange_3, orange_4, orange_5, orange_6, orange_7, orange_8, orange_9};
}

