/*
** FAAD2 - Freeware Advanced Audio (AAC) Decoder including SBR decoding
** Copyright (C) 2003-2005 M. Bakker, Nero AG, http://www.nero.com
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
** Any non-GPL usage of this software or parts of this software is strictly
** forbidden.
**
** The "appropriate copyright message" mentioned in section 2c of the GPLv2
** must read: "Code from FAAD2 is copyright (c) Nero AG, www.nero.com"
**
** Commercial non-GPL licensing of this software is possible.
** For more info contact Nero AG through Mpeg4AAClicense@nero.com.
**
** $Id: sbr_dct.c,v 1.20 2007/11/01 12:33:34 menno Exp $
**/


/* Most of the DCT/DST codes here are generated using Spiral which is GPL
 * For more info see: http://www.spiral.net/
 */

#include "common.h"

#ifdef SBR_DEC

#ifdef _MSC_VER
#pragma warning(disable:4305)
#pragma warning(disable:4244)
#endif


#include "sbr_dct.h"

real_t fc0, fc1, fc2, fc3, fc4, fc5, fc6, fc7, fc8, fc9, fc10;
real_t fc11, fc12, fc13, fc14, fc15, fc16, fc17, fc18, fc19, fc20;
real_t fc21, fc22, fc23, fc24, fc25, fc26, fc27, fc28, fc29, fc30;
real_t fc31, fc32, fc33, fc34, fc35, fc36, fc37, fc38, fc39, fc40;
real_t fc41, fc42, fc43, fc44, fc45, fc46, fc47, fc48, fc49, fc50;
real_t fc51, fc52, fc53, fc54, fc55, fc56, fc57, fc58, fc59, fc60;
real_t fc61, fc62, fc63, fc64, fc65, fc66, fc67, fc68, fc69, fc70;
real_t fc71, fc72, fc73, fc74, fc75, fc76, fc77, fc78, fc79, fc80;
real_t fc81, fc82, fc83, fc84, fc85, fc86, fc87, fc88, fc89, fc90;
real_t fc91, fc92, fc93, fc94, fc95, fc96, fc97, fc98, fc99, fc100;
real_t fc101, fc102, fc103, fc104, fc105, fc106, fc107, fc108, fc109, fc110;
real_t fc111, fc112, fc113, fc114, fc115, fc116, fc117, fc118, fc119, fc120;
real_t fc121, fc122, fc123, fc124, fc125, fc126, fc127, fc128, fc129, fc130;
real_t fc131, fc132, fc133, fc134, fc135, fc136, fc137, fc138, fc139, fc140;
real_t fc141, fc142, fc143, fc144, fc145, fc146, fc147, fc148, fc149, fc150;
real_t fc151, fc152, fc153, fc154, fc155, fc156, fc157, fc158, fc159, fc160;
real_t fc161, fc162, fc163, fc164, fc165, fc166, fc167, fc168, fc169, fc170;
real_t fc171, fc172, fc173, fc174, fc175, fc176, fc177, fc178, fc179, fc180;
real_t fc181, fc182, fc183, fc184, fc185, fc186, fc187, fc188, fc189, fc190;
real_t fc191, fc192, fc193, fc194, fc195, fc196, fc197, fc198, fc199, fc200;
real_t fc201, fc202, fc203, fc204, fc205, fc206, fc207, fc208, fc209, fc210;
real_t fc211, fc212, fc213, fc214, fc215, fc216, fc217, fc218, fc219, fc220;
real_t fc221, fc222, fc223, fc224, fc225, fc226, fc227, fc228, fc229, fc230;
real_t fc231, fc232, fc233, fc234, fc235, fc236, fc237, fc238, fc239, fc240;
real_t fc241, fc242, fc243, fc244, fc245, fc246, fc247, fc248, fc249, fc250;
real_t fc251, fc252, fc253, fc254, fc255, fc256, fc257, fc258, fc259, fc260;
real_t fc261, fc262, fc263, fc264, fc265, fc266, fc267, fc268, fc269, fc270;
real_t fc271, fc272, fc273, fc274, fc275, fc276, fc277, fc278, fc279, fc280;
real_t fc281, fc282, fc283, fc284, fc285, fc286, fc287, fc288, fc289, fc290;
real_t fc291, fc292, fc293, fc294, fc295, fc296, fc297, fc298, fc299, fc300;
real_t fc301, fc302, fc303, fc304, fc305, fc306, fc307, fc310, fc311, fc312;
real_t fc313, fc316, fc317, fc318, fc319, fc322, fc323, fc324, fc325, fc328;
real_t fc329, fc330, fc331, fc334, fc335, fc336, fc337, fc340, fc341, fc342;
real_t fc343, fc346, fc347, fc348, fc349, fc352, fc353, fc354, fc355, fc358;
real_t fc359, fc360, fc361, fc364, fc365, fc366, fc367, fc370, fc371, fc372;
real_t fc373, fc376, fc377, fc378, fc379, fc382, fc383, fc384, fc385, fc388;
real_t fc389, fc390, fc391, fc394, fc395, fc396, fc397;
void DCT4_32(real_t *y, real_t *x)
{
    fc0 = x[15] - x[16];
    fc1 = x[15] + x[16];
    fc2 = MUL_F(FRAC_CONST(0.7071067811865476), fc1);
    fc3 = MUL_F(FRAC_CONST(0.7071067811865476), fc0);
    fc4 = x[8] - x[23];
    fc5 = x[8] + x[23];
    fc6 = MUL_F(FRAC_CONST(0.7071067811865476), fc5);
    fc7 = MUL_F(FRAC_CONST(0.7071067811865476), fc4);
    fc8 = x[12] - x[19];
    fc9 = x[12] + x[19];
    fc10 = MUL_F(FRAC_CONST(0.7071067811865476), fc9);
    fc11 = MUL_F(FRAC_CONST(0.7071067811865476), fc8);
    fc12 = x[11] - x[20];
    fc13 = x[11] + x[20];
    fc14 = MUL_F(FRAC_CONST(0.7071067811865476), fc13);
    fc15 = MUL_F(FRAC_CONST(0.7071067811865476), fc12);
    fc16 = x[14] - x[17];
    fc17 = x[14] + x[17];
    fc18 = MUL_F(FRAC_CONST(0.7071067811865476), fc17);
    fc19 = MUL_F(FRAC_CONST(0.7071067811865476), fc16);
    fc20 = x[9] - x[22];
    fc21 = x[9] + x[22];
    fc22 = MUL_F(FRAC_CONST(0.7071067811865476), fc21);
    fc23 = MUL_F(FRAC_CONST(0.7071067811865476), fc20);
    fc24 = x[13] - x[18];
    fc25 = x[13] + x[18];
    fc26 = MUL_F(FRAC_CONST(0.7071067811865476), fc25);
    fc27 = MUL_F(FRAC_CONST(0.7071067811865476), fc24);
    fc28 = x[10] - x[21];
    fc29 = x[10] + x[21];
    fc30 = MUL_F(FRAC_CONST(0.7071067811865476), fc29);
    fc31 = MUL_F(FRAC_CONST(0.7071067811865476), fc28);
    fc32 = x[0] - fc2;
    fc33 = x[0] + fc2;
    fc34 = x[31] - fc3;
    fc35 = x[31] + fc3;
    fc36 = x[7] - fc6;
    fc37 = x[7] + fc6;
    fc38 = x[24] - fc7;
    fc39 = x[24] + fc7;
    fc40 = x[3] - fc10;
    fc41 = x[3] + fc10;
    fc42 = x[28] - fc11;
    fc43 = x[28] + fc11;
    fc44 = x[4] - fc14;
    fc45 = x[4] + fc14;
    fc46 = x[27] - fc15;
    fc47 = x[27] + fc15;
    fc48 = x[1] - fc18;
    fc49 = x[1] + fc18;
    fc50 = x[30] - fc19;
    fc51 = x[30] + fc19;
    fc52 = x[6] - fc22;
    fc53 = x[6] + fc22;
    fc54 = x[25] - fc23;
    fc55 = x[25] + fc23;
    fc56 = x[2] - fc26;
    fc57 = x[2] + fc26;
    fc58 = x[29] - fc27;
    fc59 = x[29] + fc27;
    fc60 = x[5] - fc30;
    fc61 = x[5] + fc30;
    fc62 = x[26] - fc31;
    fc63 = x[26] + fc31;
    fc64 = fc39 + fc37;
    fc65 = MUL_F(FRAC_CONST(-0.5411961001461969), fc39);
    fc66 = MUL_F(FRAC_CONST(0.9238795325112867), fc64);
    fc67 = MUL_C(COEF_CONST(1.3065629648763766), fc37);
    fc68 = fc65 + fc66;
    fc69 = fc67 - fc66;
    fc70 = fc38 + fc36;
    fc71 = MUL_C(COEF_CONST(1.3065629648763770), fc38);
    fc72 = MUL_F(FRAC_CONST(-0.3826834323650904), fc70);
    fc73 = MUL_F(FRAC_CONST(0.5411961001461961), fc36);
    fc74 = fc71 + fc72;
    fc75 = fc73 - fc72;
    fc76 = fc47 + fc45;
    fc77 = MUL_F(FRAC_CONST(-0.5411961001461969), fc47);
    fc78 = MUL_F(FRAC_CONST(0.9238795325112867), fc76);
    fc79 = MUL_C(COEF_CONST(1.3065629648763766), fc45);
    fc80 = fc77 + fc78;
    fc81 = fc79 - fc78;
    fc82 = fc46 + fc44;
    fc83 = MUL_C(COEF_CONST(1.3065629648763770), fc46);
    fc84 = MUL_F(FRAC_CONST(-0.3826834323650904), fc82);
    fc85 = MUL_F(FRAC_CONST(0.5411961001461961), fc44);
    fc86 = fc83 + fc84;
    fc87 = fc85 - fc84;
    fc88 = fc55 + fc53;
    fc89 = MUL_F(FRAC_CONST(-0.5411961001461969), fc55);
    fc90 = MUL_F(FRAC_CONST(0.9238795325112867), fc88);
    fc91 = MUL_C(COEF_CONST(1.3065629648763766), fc53);
    fc92 = fc89 + fc90;
    fc93 = fc91 - fc90;
    fc94 = fc54 + fc52;
    fc95 = MUL_C(COEF_CONST(1.3065629648763770), fc54);
    fc96 = MUL_F(FRAC_CONST(-0.3826834323650904), fc94);
    fc97 = MUL_F(FRAC_CONST(0.5411961001461961), fc52);
    fc98 = fc95 + fc96;
    fc99 = fc97 - fc96;
    fc100 = fc63 + fc61;
    fc101 = MUL_F(FRAC_CONST(-0.5411961001461969), fc63);
    fc102 = MUL_F(FRAC_CONST(0.9238795325112867), fc100);
    fc103 = MUL_C(COEF_CONST(1.3065629648763766), fc61);
    fc104 = fc101 + fc102;
    fc105 = fc103 - fc102;
    fc106 = fc62 + fc60;
    fc107 = MUL_C(COEF_CONST(1.3065629648763770), fc62);
    fc108 = MUL_F(FRAC_CONST(-0.3826834323650904), fc106);
    fc109 = MUL_F(FRAC_CONST(0.5411961001461961), fc60);
    fc110 = fc107 + fc108;
    fc111 = fc109 - fc108;
    fc112 = fc33 - fc68;
    fc113 = fc33 + fc68;
    fc114 = fc35 - fc69;
    fc115 = fc35 + fc69;
    fc116 = fc32 - fc74;
    fc117 = fc32 + fc74;
    fc118 = fc34 - fc75;
    fc119 = fc34 + fc75;
    fc120 = fc41 - fc80;
    fc121 = fc41 + fc80;
    fc122 = fc43 - fc81;
    fc123 = fc43 + fc81;
    fc124 = fc40 - fc86;
    fc125 = fc40 + fc86;
    fc126 = fc42 - fc87;
    fc127 = fc42 + fc87;
    fc128 = fc49 - fc92;
    fc129 = fc49 + fc92;
    fc130 = fc51 - fc93;
    fc131 = fc51 + fc93;
    fc132 = fc48 - fc98;
    fc133 = fc48 + fc98;
    fc134 = fc50 - fc99;
    fc135 = fc50 + fc99;
    fc136 = fc57 - fc104;
    fc137 = fc57 + fc104;
    fc138 = fc59 - fc105;
    fc139 = fc59 + fc105;
    fc140 = fc56 - fc110;
    fc141 = fc56 + fc110;
    fc142 = fc58 - fc111;
    fc143 = fc58 + fc111;
    fc144 = fc123 + fc121;
    fc145 = MUL_F(FRAC_CONST(-0.7856949583871021), fc123);
    fc146 = MUL_F(FRAC_CONST(0.9807852804032304), fc144);
    fc147 = MUL_C(COEF_CONST(1.1758756024193588), fc121);
    fc148 = fc145 + fc146;
    fc149 = fc147 - fc146;
    fc150 = fc127 + fc125;
    fc151 = MUL_F(FRAC_CONST(0.2758993792829431), fc127);
    fc152 = MUL_F(FRAC_CONST(0.5555702330196022), fc150);
    fc153 = MUL_C(COEF_CONST(1.3870398453221475), fc125);
    fc154 = fc151 + fc152;
    fc155 = fc153 - fc152;
    fc156 = fc122 + fc120;
    fc157 = MUL_C(COEF_CONST(1.1758756024193591), fc122);
    fc158 = MUL_F(FRAC_CONST(-0.1950903220161287), fc156);
    fc159 = MUL_F(FRAC_CONST(0.7856949583871016), fc120);
    fc160 = fc157 + fc158;
    fc161 = fc159 - fc158;
    fc162 = fc126 + fc124;
    fc163 = MUL_C(COEF_CONST(1.3870398453221473), fc126);
    fc164 = MUL_F(FRAC_CONST(-0.8314696123025455), fc162);
    fc165 = MUL_F(FRAC_CONST(-0.2758993792829436), fc124);
    fc166 = fc163 + fc164;
    fc167 = fc165 - fc164;
    fc168 = fc139 + fc137;
    fc169 = MUL_F(FRAC_CONST(-0.7856949583871021), fc139);
    fc170 = MUL_F(FRAC_CONST(0.9807852804032304), fc168);
    fc171 = MUL_C(COEF_CONST(1.1758756024193588), fc137);
    fc172 = fc169 + fc170;
    fc173 = fc171 - fc170;
    fc174 = fc143 + fc141;
    fc175 = MUL_F(FRAC_CONST(0.2758993792829431), fc143);
    fc176 = MUL_F(FRAC_CONST(0.5555702330196022), fc174);
    fc177 = MUL_C(COEF_CONST(1.3870398453221475), fc141);
    fc178 = fc175 + fc176;
    fc179 = fc177 - fc176;
    fc180 = fc138 + fc136;
    fc181 = MUL_C(COEF_CONST(1.1758756024193591), fc138);
    fc182 = MUL_F(FRAC_CONST(-0.1950903220161287), fc180);
    fc183 = MUL_F(FRAC_CONST(0.7856949583871016), fc136);
    fc184 = fc181 + fc182;
    fc185 = fc183 - fc182;
    fc186 = fc142 + fc140;
    fc187 = MUL_C(COEF_CONST(1.3870398453221473), fc142);
    fc188 = MUL_F(FRAC_CONST(-0.8314696123025455), fc186);
    fc189 = MUL_F(FRAC_CONST(-0.2758993792829436), fc140);
    fc190 = fc187 + fc188;
    fc191 = fc189 - fc188;
    fc192 = fc113 - fc148;
    fc193 = fc113 + fc148;
    fc194 = fc115 - fc149;
    fc195 = fc115 + fc149;
    fc196 = fc117 - fc154;
    fc197 = fc117 + fc154;
    fc198 = fc119 - fc155;
    fc199 = fc119 + fc155;
    fc200 = fc112 - fc160;
    fc201 = fc112 + fc160;
    fc202 = fc114 - fc161;
    fc203 = fc114 + fc161;
    fc204 = fc116 - fc166;
    fc205 = fc116 + fc166;
    fc206 = fc118 - fc167;
    fc207 = fc118 + fc167;
    fc208 = fc129 - fc172;
    fc209 = fc129 + fc172;
    fc210 = fc131 - fc173;
    fc211 = fc131 + fc173;
    fc212 = fc133 - fc178;
    fc213 = fc133 + fc178;
    fc214 = fc135 - fc179;
    fc215 = fc135 + fc179;
    fc216 = fc128 - fc184;
    fc217 = fc128 + fc184;
    fc218 = fc130 - fc185;
    fc219 = fc130 + fc185;
    fc220 = fc132 - fc190;
    fc221 = fc132 + fc190;
    fc222 = fc134 - fc191;
    fc223 = fc134 + fc191;
    fc224 = fc211 + fc209;
    fc225 = MUL_F(FRAC_CONST(-0.8971675863426361), fc211);
    fc226 = MUL_F(FRAC_CONST(0.9951847266721968), fc224);
    fc227 = MUL_C(COEF_CONST(1.0932018670017576), fc209);
    fc228 = fc225 + fc226;
    fc229 = fc227 - fc226;
    fc230 = fc215 + fc213;
    fc231 = MUL_F(FRAC_CONST(-0.4105245275223571), fc215);
    fc232 = MUL_F(FRAC_CONST(0.8819212643483549), fc230);
    fc233 = MUL_C(COEF_CONST(1.3533180011743529), fc213);
    fc234 = fc231 + fc232;
    fc235 = fc233 - fc232;
    fc236 = fc219 + fc217;
    fc237 = MUL_F(FRAC_CONST(0.1386171691990915), fc219);
    fc238 = MUL_F(FRAC_CONST(0.6343932841636455), fc236);
    fc239 = MUL_C(COEF_CONST(1.4074037375263826), fc217);
    fc240 = fc237 + fc238;
    fc241 = fc239 - fc238;
    fc242 = fc223 + fc221;
    fc243 = MUL_F(FRAC_CONST(0.6666556584777466), fc223);
    fc244 = MUL_F(FRAC_CONST(0.2902846772544623), fc242);
    fc245 = MUL_C(COEF_CONST(1.2472250129866711), fc221);
    fc246 = fc243 + fc244;
    fc247 = fc245 - fc244;
    fc248 = fc210 + fc208;
    fc249 = MUL_C(COEF_CONST(1.0932018670017574), fc210);
    fc250 = MUL_F(FRAC_CONST(-0.0980171403295605), fc248);
    fc251 = MUL_F(FRAC_CONST(0.8971675863426364), fc208);
    fc252 = fc249 + fc250;
    fc253 = fc251 - fc250;
    fc254 = fc214 + fc212;
    fc255 = MUL_C(COEF_CONST(1.3533180011743529), fc214);
    fc256 = MUL_F(FRAC_CONST(-0.4713967368259979), fc254);
    fc257 = MUL_F(FRAC_CONST(0.4105245275223569), fc212);
    fc258 = fc255 + fc256;
    fc259 = fc257 - fc256;
    fc260 = fc218 + fc216;
    fc261 = MUL_C(COEF_CONST(1.4074037375263826), fc218);
    fc262 = MUL_F(FRAC_CONST(-0.7730104533627369), fc260);
    fc263 = MUL_F(FRAC_CONST(-0.1386171691990913), fc216);
    fc264 = fc261 + fc262;
    fc265 = fc263 - fc262;
    fc266 = fc222 + fc220;
    fc267 = MUL_C(COEF_CONST(1.2472250129866711), fc222);
    fc268 = MUL_F(FRAC_CONST(-0.9569403357322089), fc266);
    fc269 = MUL_F(FRAC_CONST(-0.6666556584777469), fc220);
    fc270 = fc267 + fc268;
    fc271 = fc269 - fc268;
    fc272 = fc193 - fc228;
    fc273 = fc193 + fc228;
    fc274 = fc195 - fc229;
    fc275 = fc195 + fc229;
    fc276 = fc197 - fc234;
    fc277 = fc197 + fc234;
    fc278 = fc199 - fc235;
    fc279 = fc199 + fc235;
    fc280 = fc201 - fc240;
    fc281 = fc201 + fc240;
    fc282 = fc203 - fc241;
    fc283 = fc203 + fc241;
    fc284 = fc205 - fc246;
    fc285 = fc205 + fc246;
    fc286 = fc207 - fc247;
    fc287 = fc207 + fc247;
    fc288 = fc192 - fc252;
    fc289 = fc192 + fc252;
    fc290 = fc194 - fc253;
    fc291 = fc194 + fc253;
    fc292 = fc196 - fc258;
    fc293 = fc196 + fc258;
    fc294 = fc198 - fc259;
    fc295 = fc198 + fc259;
    fc296 = fc200 - fc264;
    fc297 = fc200 + fc264;
    fc298 = fc202 - fc265;
    fc299 = fc202 + fc265;
    fc300 = fc204 - fc270;
    fc301 = fc204 + fc270;
    fc302 = fc206 - fc271;
    fc303 = fc206 + fc271;
    fc304 = fc275 + fc273;
    fc305 = MUL_F(FRAC_CONST(-0.9751575901732920), fc275);
    fc306 = MUL_F(FRAC_CONST(0.9996988186962043), fc304);
    fc307 = MUL_C(COEF_CONST(1.0242400472191164), fc273);
    y[0] = fc305 + fc306;
    y[31] = fc307 - fc306;
    fc310 = fc279 + fc277;
    fc311 = MUL_F(FRAC_CONST(-0.8700688593994936), fc279);
    fc312 = MUL_F(FRAC_CONST(0.9924795345987100), fc310);
    fc313 = MUL_C(COEF_CONST(1.1148902097979263), fc277);
    y[2] = fc311 + fc312;
    y[29] = fc313 - fc312;
    fc316 = fc283 + fc281;
    fc317 = MUL_F(FRAC_CONST(-0.7566008898816587), fc283);
    fc318 = MUL_F(FRAC_CONST(0.9757021300385286), fc316);
    fc319 = MUL_C(COEF_CONST(1.1948033701953984), fc281);
    y[4] = fc317 + fc318;
    y[27] = fc319 - fc318;
    fc322 = fc287 + fc285;
    fc323 = MUL_F(FRAC_CONST(-0.6358464401941451), fc287);
    fc324 = MUL_F(FRAC_CONST(0.9495281805930367), fc322);
    fc325 = MUL_C(COEF_CONST(1.2632099209919283), fc285);
    y[6] = fc323 + fc324;
    y[25] = fc325 - fc324;
    fc328 = fc291 + fc289;
    fc329 = MUL_F(FRAC_CONST(-0.5089684416985408), fc291);
    fc330 = MUL_F(FRAC_CONST(0.9142097557035307), fc328);
    fc331 = MUL_C(COEF_CONST(1.3194510697085207), fc289);
    y[8] = fc329 + fc330;
    y[23] = fc331 - fc330;
    fc334 = fc295 + fc293;
    fc335 = MUL_F(FRAC_CONST(-0.3771887988789273), fc295);
    fc336 = MUL_F(FRAC_CONST(0.8700869911087114), fc334);
    fc337 = MUL_C(COEF_CONST(1.3629851833384954), fc293);
    y[10] = fc335 + fc336;
    y[21] = fc337 - fc336;
    fc340 = fc299 + fc297;
    fc341 = MUL_F(FRAC_CONST(-0.2417766217337384), fc299);
    fc342 = MUL_F(FRAC_CONST(0.8175848131515837), fc340);
    fc343 = MUL_C(COEF_CONST(1.3933930045694289), fc297);
    y[12] = fc341 + fc342;
    y[19] = fc343 - fc342;
    fc346 = fc303 + fc301;
    fc347 = MUL_F(FRAC_CONST(-0.1040360035527077), fc303);
    fc348 = MUL_F(FRAC_CONST(0.7572088465064845), fc346);
    fc349 = MUL_C(COEF_CONST(1.4103816894602612), fc301);
    y[14] = fc347 + fc348;
    y[17] = fc349 - fc348;
    fc352 = fc274 + fc272;
    fc353 = MUL_F(FRAC_CONST(0.0347065382144002), fc274);
    fc354 = MUL_F(FRAC_CONST(0.6895405447370668), fc352);
    fc355 = MUL_C(COEF_CONST(1.4137876276885337), fc272);
    y[16] = fc353 + fc354;
    y[15] = fc355 - fc354;
    fc358 = fc278 + fc276;
    fc359 = MUL_F(FRAC_CONST(0.1731148370459795), fc278);
    fc360 = MUL_F(FRAC_CONST(0.6152315905806268), fc358);
    fc361 = MUL_C(COEF_CONST(1.4035780182072330), fc276);
    y[18] = fc359 + fc360;
    y[13] = fc361 - fc360;
    fc364 = fc282 + fc280;
    fc365 = MUL_F(FRAC_CONST(0.3098559453626100), fc282);
    fc366 = MUL_F(FRAC_CONST(0.5349976198870972), fc364);
    fc367 = MUL_C(COEF_CONST(1.3798511851368043), fc280);
    y[20] = fc365 + fc366;
    y[11] = fc367 - fc366;
    fc370 = fc286 + fc284;
    fc371 = MUL_F(FRAC_CONST(0.4436129715409088), fc286);
    fc372 = MUL_F(FRAC_CONST(0.4496113296546065), fc370);
    fc373 = MUL_C(COEF_CONST(1.3428356308501219), fc284);
    y[22] = fc371 + fc372;
    y[9] = fc373 - fc372;
    fc376 = fc290 + fc288;
    fc377 = MUL_F(FRAC_CONST(0.5730977622997509), fc290);
    fc378 = MUL_F(FRAC_CONST(0.3598950365349881), fc376);
    fc379 = MUL_C(COEF_CONST(1.2928878353697271), fc288);
    y[24] = fc377 + fc378;
    y[7] = fc379 - fc378;
    fc382 = fc294 + fc292;
    fc383 = MUL_F(FRAC_CONST(0.6970633083205415), fc294);
    fc384 = MUL_F(FRAC_CONST(0.2667127574748984), fc382);
    fc385 = MUL_C(COEF_CONST(1.2304888232703382), fc292);
    y[26] = fc383 + fc384;
    y[5] = fc385 - fc384;
    fc388 = fc298 + fc296;
    fc389 = MUL_F(FRAC_CONST(0.8143157536286401), fc298);
    fc390 = MUL_F(FRAC_CONST(0.1709618887603012), fc388);
    fc391 = MUL_C(COEF_CONST(1.1562395311492424), fc296);
    y[28] = fc389 + fc390;
    y[3] = fc391 - fc390;
    fc394 = fc302 + fc300;
    fc395 = MUL_F(FRAC_CONST(0.9237258930790228), fc302);
    fc396 = MUL_F(FRAC_CONST(0.0735645635996674), fc394);
    fc397 = MUL_C(COEF_CONST(1.0708550202783576), fc300);
    y[30] = fc395 + fc396;
    y[1] = fc397 - fc396;
}

real_t fs0, fs1, fs2, fs3, fs4, fs5, fs6, fs7, fs8, fs9;
real_t fs10, fs11, fs12, fs13, fs14, fs15, fs16, fs17, fs18, fs19;
real_t fs20, fs21, fs22, fs23, fs24, fs25, fs26, fs27, fs28, fs29;
real_t fs30, fs31, fs32, fs33, fs34, fs35, fs36, fs37, fs38, fs39;
real_t fs40, fs41, fs42, fs43, fs44, fs45, fs46, fs47, fs48, fs49;
real_t fs50, fs51, fs52, fs53, fs54, fs55, fs56, fs57, fs58, fs59;
real_t fs60, fs61, fs62, fs63, fs64, fs65, fs66, fs67, fs68, fs69;
real_t fs70, fs71, fs72, fs73, fs74, fs75, fs76, fs77, fs78, fs79;
real_t fs80, fs81, fs82, fs83, fs84, fs85, fs86, fs87, fs88, fs89;
real_t fs90, fs91, fs92, fs93, fs94, fs95, fs96, fs97, fs98, fs99;
real_t fs100, fs101, fs102, fs103, fs104, fs105, fs106, fs107, fs108, fs109;
real_t fs110, fs111, fs112, fs113, fs114, fs115, fs116, fs117, fs118, fs119;
real_t fs120, fs121, fs122, fs123, fs124, fs125, fs126, fs127, fs128, fs129;
real_t fs130, fs131, fs132, fs133, fs134, fs135, fs136, fs137, fs138, fs139;
real_t fs140, fs141, fs142, fs143, fs144, fs145, fs146, fs147, fs148, fs149;
real_t fs150, fs151, fs152, fs153, fs154, fs155, fs156, fs157, fs158, fs159;
real_t fs160, fs161, fs162, fs163, fs164, fs165, fs166, fs167, fs168, fs169;
real_t fs170, fs171, fs172, fs173, fs174, fs175, fs176, fs177, fs178, fs179;
real_t fs180, fs181, fs182, fs183, fs184, fs185, fs186, fs187, fs188, fs189;
real_t fs190, fs191, fs192, fs193, fs194, fs195, fs196, fs197, fs198, fs199;
real_t fs200, fs201, fs202, fs203, fs204, fs205, fs206, fs207, fs208, fs209;
real_t fs210, fs211, fs212, fs213, fs214, fs215, fs216, fs217, fs218, fs219;
real_t fs220, fs221, fs222, fs223, fs224, fs225, fs226, fs227, fs228, fs229;
real_t fs230, fs231, fs232, fs233, fs234, fs235, fs236, fs237, fs238, fs239;
real_t fs240, fs241, fs242, fs243, fs244, fs245, fs246, fs247, fs248, fs249;
real_t fs250, fs251, fs252, fs253, fs254, fs255, fs256, fs257, fs258, fs259;
real_t fs260, fs261, fs262, fs263, fs264, fs265, fs266, fs267, fs268, fs269;
real_t fs270, fs271, fs272, fs273, fs274, fs275, fs276, fs277, fs278, fs279;
real_t fs280, fs281, fs282, fs283, fs284, fs285, fs286, fs287, fs288, fs289;
real_t fs290, fs291, fs292, fs293, fs294, fs295, fs296, fs297, fs298, fs299;
real_t fs300, fs301, fs302, fs303, fs304, fs305, fs306, fs307, fs308, fs309;
real_t fs310, fs311, fs312, fs313, fs314, fs315, fs316, fs317, fs318, fs319;
real_t fs320, fs321, fs322, fs323, fs324, fs325, fs326, fs327, fs328, fs329;
real_t fs330, fs331, fs332, fs333, fs334, fs335;
void DST4_32(real_t *y, real_t *x)
{
    fs0 = x[0] - x[1];
    fs1 = x[2] - x[1];
    fs2 = x[2] - x[3];
    fs3 = x[4] - x[3];
    fs4 = x[4] - x[5];
    fs5 = x[6] - x[5];
    fs6 = x[6] - x[7];
    fs7 = x[8] - x[7];
    fs8 = x[8] - x[9];
    fs9 = x[10] - x[9];
    fs10 = x[10] - x[11];
    fs11 = x[12] - x[11];
    fs12 = x[12] - x[13];
    fs13 = x[14] - x[13];
    fs14 = x[14] - x[15];
    fs15 = x[16] - x[15];
    fs16 = x[16] - x[17];
    fs17 = x[18] - x[17];
    fs18 = x[18] - x[19];
    fs19 = x[20] - x[19];
    fs20 = x[20] - x[21];
    fs21 = x[22] - x[21];
    fs22 = x[22] - x[23];
    fs23 = x[24] - x[23];
    fs24 = x[24] - x[25];
    fs25 = x[26] - x[25];
    fs26 = x[26] - x[27];
    fs27 = x[28] - x[27];
    fs28 = x[28] - x[29];
    fs29 = x[30] - x[29];
    fs30 = x[30] - x[31];
    fs31 = MUL_F(FRAC_CONST(0.7071067811865476), fs15);
    fs32 = x[0] - fs31;
    fs33 = x[0] + fs31;
    fs34 = fs7 + fs23;
    fs35 = MUL_C(COEF_CONST(1.3065629648763766), fs7);
    fs36 = MUL_F(FRAC_CONST(-0.9238795325112866), fs34);
    fs37 = MUL_F(FRAC_CONST(-0.5411961001461967), fs23);
    fs38 = fs35 + fs36;
    fs39 = fs37 - fs36;
    fs40 = fs33 - fs39;
    fs41 = fs33 + fs39;
    fs42 = fs32 - fs38;
    fs43 = fs32 + fs38;
    fs44 = fs11 - fs19;
    fs45 = fs11 + fs19;
    fs46 = MUL_F(FRAC_CONST(0.7071067811865476), fs45);
    fs47 = fs3 - fs46;
    fs48 = fs3 + fs46;
    fs49 = MUL_F(FRAC_CONST(0.7071067811865476), fs44);
    fs50 = fs49 - fs27;
    fs51 = fs49 + fs27;
    fs52 = fs51 + fs48;
    fs53 = MUL_F(FRAC_CONST(-0.7856949583871021), fs51);
    fs54 = MUL_F(FRAC_CONST(0.9807852804032304), fs52);
    fs55 = MUL_C(COEF_CONST(1.1758756024193588), fs48);
    fs56 = fs53 + fs54;
    fs57 = fs55 - fs54;
    fs58 = fs50 + fs47;
    fs59 = MUL_F(FRAC_CONST(-0.2758993792829430), fs50);
    fs60 = MUL_F(FRAC_CONST(0.8314696123025452), fs58);
    fs61 = MUL_C(COEF_CONST(1.3870398453221475), fs47);
    fs62 = fs59 + fs60;
    fs63 = fs61 - fs60;
    fs64 = fs41 - fs56;
    fs65 = fs41 + fs56;
    fs66 = fs43 - fs62;
    fs67 = fs43 + fs62;
    fs68 = fs42 - fs63;
    fs69 = fs42 + fs63;
    fs70 = fs40 - fs57;
    fs71 = fs40 + fs57;
    fs72 = fs5 - fs9;
    fs73 = fs5 + fs9;
    fs74 = fs13 - fs17;
    fs75 = fs13 + fs17;
    fs76 = fs21 - fs25;
    fs77 = fs21 + fs25;
    fs78 = MUL_F(FRAC_CONST(0.7071067811865476), fs75);
    fs79 = fs1 - fs78;
    fs80 = fs1 + fs78;
    fs81 = fs73 + fs77;
    fs82 = MUL_C(COEF_CONST(1.3065629648763766), fs73);
    fs83 = MUL_F(FRAC_CONST(-0.9238795325112866), fs81);
    fs84 = MUL_F(FRAC_CONST(-0.5411961001461967), fs77);
    fs85 = fs82 + fs83;
    fs86 = fs84 - fs83;
    fs87 = fs80 - fs86;
    fs88 = fs80 + fs86;
    fs89 = fs79 - fs85;
    fs90 = fs79 + fs85;
    fs91 = MUL_F(FRAC_CONST(0.7071067811865476), fs74);
    fs92 = fs29 - fs91;
    fs93 = fs29 + fs91;
    fs94 = fs76 + fs72;
    fs95 = MUL_C(COEF_CONST(1.3065629648763766), fs76);
    fs96 = MUL_F(FRAC_CONST(-0.9238795325112866), fs94);
    fs97 = MUL_F(FRAC_CONST(-0.5411961001461967), fs72);
    fs98 = fs95 + fs96;
    fs99 = fs97 - fs96;
    fs100 = fs93 - fs99;
    fs101 = fs93 + fs99;
    fs102 = fs92 - fs98;
    fs103 = fs92 + fs98;
    fs104 = fs101 + fs88;
    fs105 = MUL_F(FRAC_CONST(-0.8971675863426361), fs101);
    fs106 = MUL_F(FRAC_CONST(0.9951847266721968), fs104);
    fs107 = MUL_C(COEF_CONST(1.0932018670017576), fs88);
    fs108 = fs105 + fs106;
    fs109 = fs107 - fs106;
    fs110 = fs90 - fs103;
    fs111 = MUL_F(FRAC_CONST(-0.6666556584777466), fs103);
    fs112 = MUL_F(FRAC_CONST(0.9569403357322089), fs110);
    fs113 = MUL_C(COEF_CONST(1.2472250129866713), fs90);
    fs114 = fs112 - fs111;
    fs115 = fs113 - fs112;
    fs116 = fs102 + fs89;
    fs117 = MUL_F(FRAC_CONST(-0.4105245275223571), fs102);
    fs118 = MUL_F(FRAC_CONST(0.8819212643483549), fs116);
    fs119 = MUL_C(COEF_CONST(1.3533180011743529), fs89);
    fs120 = fs117 + fs118;
    fs121 = fs119 - fs118;
    fs122 = fs87 - fs100;
    fs123 = MUL_F(FRAC_CONST(-0.1386171691990915), fs100);
    fs124 = MUL_F(FRAC_CONST(0.7730104533627370), fs122);
    fs125 = MUL_C(COEF_CONST(1.4074037375263826), fs87);
    fs126 = fs124 - fs123;
    fs127 = fs125 - fs124;
    fs128 = fs65 - fs108;
    fs129 = fs65 + fs108;
    fs130 = fs67 - fs114;
    fs131 = fs67 + fs114;
    fs132 = fs69 - fs120;
    fs133 = fs69 + fs120;
    fs134 = fs71 - fs126;
    fs135 = fs71 + fs126;
    fs136 = fs70 - fs127;
    fs137 = fs70 + fs127;
    fs138 = fs68 - fs121;
    fs139 = fs68 + fs121;
    fs140 = fs66 - fs115;
    fs141 = fs66 + fs115;
    fs142 = fs64 - fs109;
    fs143 = fs64 + fs109;
    fs144 = fs0 + fs30;
    fs145 = MUL_C(COEF_CONST(1.0478631305325901), fs0);
    fs146 = MUL_F(FRAC_CONST(-0.9987954562051724), fs144);
    fs147 = MUL_F(FRAC_CONST(-0.9497277818777548), fs30);
    fs148 = fs145 + fs146;
    fs149 = fs147 - fs146;
    fs150 = fs4 + fs26;
    fs151 = MUL_F(FRAC_CONST(1.2130114330978077), fs4);
    fs152 = MUL_F(FRAC_CONST(-0.9700312531945440), fs150);
    fs153 = MUL_F(FRAC_CONST(-0.7270510732912803), fs26);
    fs154 = fs151 + fs152;
    fs155 = fs153 - fs152;
    fs156 = fs8 + fs22;
    fs157 = MUL_C(COEF_CONST(1.3315443865537255), fs8);
    fs158 = MUL_F(FRAC_CONST(-0.9039892931234433), fs156);
    fs159 = MUL_F(FRAC_CONST(-0.4764341996931612), fs22);
    fs160 = fs157 + fs158;
    fs161 = fs159 - fs158;
    fs162 = fs12 + fs18;
    fs163 = MUL_C(COEF_CONST(1.3989068359730781), fs12);
    fs164 = MUL_F(FRAC_CONST(-0.8032075314806453), fs162);
    fs165 = MUL_F(FRAC_CONST(-0.2075082269882124), fs18);
    fs166 = fs163 + fs164;
    fs167 = fs165 - fs164;
    fs168 = fs16 + fs14;
    fs169 = MUL_C(COEF_CONST(1.4125100802019777), fs16);
    fs170 = MUL_F(FRAC_CONST(-0.6715589548470187), fs168);
    fs171 = MUL_F(FRAC_CONST(0.0693921705079402), fs14);
    fs172 = fs169 + fs170;
    fs173 = fs171 - fs170;
    fs174 = fs20 + fs10;
    fs175 = MUL_C(COEF_CONST(1.3718313541934939), fs20);
    fs176 = MUL_F(FRAC_CONST(-0.5141027441932219), fs174);
    fs177 = MUL_F(FRAC_CONST(0.3436258658070501), fs10);
    fs178 = fs175 + fs176;
    fs179 = fs177 - fs176;
    fs180 = fs24 + fs6;
    fs181 = MUL_C(COEF_CONST(1.2784339185752409), fs24);
    fs182 = MUL_F(FRAC_CONST(-0.3368898533922200), fs180);
    fs183 = MUL_F(FRAC_CONST(0.6046542117908008), fs6);
    fs184 = fs181 + fs182;
    fs185 = fs183 - fs182;
    fs186 = fs28 + fs2;
    fs187 = MUL_C(COEF_CONST(1.1359069844201433), fs28);
    fs188 = MUL_F(FRAC_CONST(-0.1467304744553624), fs186);
    fs189 = MUL_F(FRAC_CONST(0.8424460355094185), fs2);
    fs190 = fs187 + fs188;
    fs191 = fs189 - fs188;
    fs192 = fs149 - fs173;
    fs193 = fs149 + fs173;
    fs194 = fs148 - fs172;
    fs195 = fs148 + fs172;
    fs196 = fs155 - fs179;
    fs197 = fs155 + fs179;
    fs198 = fs154 - fs178;
    fs199 = fs154 + fs178;
    fs200 = fs161 - fs185;
    fs201 = fs161 + fs185;
    fs202 = fs160 - fs184;
    fs203 = fs160 + fs184;
    fs204 = fs167 - fs191;
    fs205 = fs167 + fs191;
    fs206 = fs166 - fs190;
    fs207 = fs166 + fs190;
    fs208 = fs192 + fs194;
    fs209 = MUL_C(COEF_CONST(1.1758756024193588), fs192);
    fs210 = MUL_F(FRAC_CONST(-0.9807852804032304), fs208);
    fs211 = MUL_F(FRAC_CONST(-0.7856949583871021), fs194);
    fs212 = fs209 + fs210;
    fs213 = fs211 - fs210;
    fs214 = fs196 + fs198;
    fs215 = MUL_C(COEF_CONST(1.3870398453221475), fs196);
    fs216 = MUL_F(FRAC_CONST(-0.5555702330196022), fs214);
    fs217 = MUL_F(FRAC_CONST(0.2758993792829431), fs198);
    fs218 = fs215 + fs216;
    fs219 = fs217 - fs216;
    fs220 = fs200 + fs202;
    fs221 = MUL_F(FRAC_CONST(0.7856949583871022), fs200);
    fs222 = MUL_F(FRAC_CONST(0.1950903220161283), fs220);
    fs223 = MUL_C(COEF_CONST(1.1758756024193586), fs202);
    fs224 = fs221 + fs222;
    fs225 = fs223 - fs222;
    fs226 = fs204 + fs206;
    fs227 = MUL_F(FRAC_CONST(-0.2758993792829430), fs204);
    fs228 = MUL_F(FRAC_CONST(0.8314696123025452), fs226);
    fs229 = MUL_C(COEF_CONST(1.3870398453221475), fs206);
    fs230 = fs227 + fs228;
    fs231 = fs229 - fs228;
    fs232 = fs193 - fs201;
    fs233 = fs193 + fs201;
    fs234 = fs195 - fs203;
    fs235 = fs195 + fs203;
    fs236 = fs197 - fs205;
    fs237 = fs197 + fs205;
    fs238 = fs199 - fs207;
    fs239 = fs199 + fs207;
    fs240 = fs213 - fs225;
    fs241 = fs213 + fs225;
    fs242 = fs212 - fs224;
    fs243 = fs212 + fs224;
    fs244 = fs219 - fs231;
    fs245 = fs219 + fs231;
    fs246 = fs218 - fs230;
    fs247 = fs218 + fs230;
    fs248 = fs232 + fs234;
    fs249 = MUL_C(COEF_CONST(1.3065629648763766), fs232);
    fs250 = MUL_F(FRAC_CONST(-0.9238795325112866), fs248);
    fs251 = MUL_F(FRAC_CONST(-0.5411961001461967), fs234);
    fs252 = fs249 + fs250;
    fs253 = fs251 - fs250;
    fs254 = fs236 + fs238;
    fs255 = MUL_F(FRAC_CONST(0.5411961001461969), fs236);
    fs256 = MUL_F(FRAC_CONST(0.3826834323650898), fs254);
    fs257 = MUL_C(COEF_CONST(1.3065629648763766), fs238);
    fs258 = fs255 + fs256;
    fs259 = fs257 - fs256;
    fs260 = fs240 + fs242;
    fs261 = MUL_C(COEF_CONST(1.3065629648763766), fs240);
    fs262 = MUL_F(FRAC_CONST(-0.9238795325112866), fs260);
    fs263 = MUL_F(FRAC_CONST(-0.5411961001461967), fs242);
    fs264 = fs261 + fs262;
    fs265 = fs263 - fs262;
    fs266 = fs244 + fs246;
    fs267 = MUL_F(FRAC_CONST(0.5411961001461969), fs244);
    fs268 = MUL_F(FRAC_CONST(0.3826834323650898), fs266);
    fs269 = MUL_C(COEF_CONST(1.3065629648763766), fs246);
    fs270 = fs267 + fs268;
    fs271 = fs269 - fs268;
    fs272 = fs233 - fs237;
    fs273 = fs233 + fs237;
    fs274 = fs235 - fs239;
    fs275 = fs235 + fs239;
    fs276 = fs253 - fs259;
    fs277 = fs253 + fs259;
    fs278 = fs252 - fs258;
    fs279 = fs252 + fs258;
    fs280 = fs241 - fs245;
    fs281 = fs241 + fs245;
    fs282 = fs243 - fs247;
    fs283 = fs243 + fs247;
    fs284 = fs265 - fs271;
    fs285 = fs265 + fs271;
    fs286 = fs264 - fs270;
    fs287 = fs264 + fs270;
    fs288 = fs272 - fs274;
    fs289 = fs272 + fs274;
    fs290 = MUL_F(FRAC_CONST(0.7071067811865474), fs288);
    fs291 = MUL_F(FRAC_CONST(0.7071067811865474), fs289);
    fs292 = fs276 - fs278;
    fs293 = fs276 + fs278;
    fs294 = MUL_F(FRAC_CONST(0.7071067811865474), fs292);
    fs295 = MUL_F(FRAC_CONST(0.7071067811865474), fs293);
    fs296 = fs280 - fs282;
    fs297 = fs280 + fs282;
    fs298 = MUL_F(FRAC_CONST(0.7071067811865474), fs296);
    fs299 = MUL_F(FRAC_CONST(0.7071067811865474), fs297);
    fs300 = fs284 - fs286;
    fs301 = fs284 + fs286;
    fs302 = MUL_F(FRAC_CONST(0.7071067811865474), fs300);
    fs303 = MUL_F(FRAC_CONST(0.7071067811865474), fs301);
    fs304 = fs129 - fs273;
    fs305 = fs129 + fs273;
    fs306 = fs131 - fs281;
    fs307 = fs131 + fs281;
    fs308 = fs133 - fs285;
    fs309 = fs133 + fs285;
    fs310 = fs135 - fs277;
    fs311 = fs135 + fs277;
    fs312 = fs137 - fs295;
    fs313 = fs137 + fs295;
    fs314 = fs139 - fs303;
    fs315 = fs139 + fs303;
    fs316 = fs141 - fs299;
    fs317 = fs141 + fs299;
    fs318 = fs143 - fs291;
    fs319 = fs143 + fs291;
    fs320 = fs142 - fs290;
    fs321 = fs142 + fs290;
    fs322 = fs140 - fs298;
    fs323 = fs140 + fs298;
    fs324 = fs138 - fs302;
    fs325 = fs138 + fs302;
    fs326 = fs136 - fs294;
    fs327 = fs136 + fs294;
    fs328 = fs134 - fs279;
    fs329 = fs134 + fs279;
    fs330 = fs132 - fs287;
    fs331 = fs132 + fs287;
    fs332 = fs130 - fs283;
    fs333 = fs130 + fs283;
    fs334 = fs128 - fs275;
    fs335 = fs128 + fs275;
    y[31] = MUL_F(FRAC_CONST(0.5001506360206510), fs305);
    y[30] = MUL_F(FRAC_CONST(0.5013584524464084), fs307);
    y[29] = MUL_F(FRAC_CONST(0.5037887256810443), fs309);
    y[28] = MUL_F(FRAC_CONST(0.5074711720725553), fs311);
    y[27] = MUL_F(FRAC_CONST(0.5124514794082247), fs313);
    y[26] = MUL_F(FRAC_CONST(0.5187927131053328), fs315);
    y[25] = MUL_F(FRAC_CONST(0.5265773151542700), fs317);
    y[24] = MUL_F(FRAC_CONST(0.5359098169079920), fs319);
    y[23] = MUL_F(FRAC_CONST(0.5469204379855088), fs321);
    y[22] = MUL_F(FRAC_CONST(0.5597698129470802), fs323);
    y[21] = MUL_F(FRAC_CONST(0.5746551840326600), fs325);
    y[20] = MUL_F(FRAC_CONST(0.5918185358574165), fs327);
    y[19] = MUL_F(FRAC_CONST(0.6115573478825099), fs329);
    y[18] = MUL_F(FRAC_CONST(0.6342389366884031), fs331);
    y[17] = MUL_F(FRAC_CONST(0.6603198078137061), fs333);
    y[16] = MUL_F(FRAC_CONST(0.6903721282002123), fs335);
    y[15] = MUL_F(FRAC_CONST(0.7251205223771985), fs334);
    y[14] = MUL_F(FRAC_CONST(0.7654941649730891), fs332);
    y[13] = MUL_F(FRAC_CONST(0.8127020908144905), fs330);
    y[12] = MUL_F(FRAC_CONST(0.8683447152233481), fs328);
    y[11] = MUL_F(FRAC_CONST(0.9345835970364075), fs326);
    y[10] = MUL_C(COEF_CONST(1.0144082649970547), fs324);
    y[9] = MUL_C(COEF_CONST(1.1120716205797176), fs322);
    y[8] = MUL_C(COEF_CONST(1.2338327379765710), fs320);
    y[7] = MUL_C(COEF_CONST(1.3892939586328277), fs318);
    y[6] = MUL_C(COEF_CONST(1.5939722833856311), fs316);
    y[5] = MUL_C(COEF_CONST(1.8746759800084078), fs314);
    y[4] = MUL_C(COEF_CONST(2.2820500680051619), fs312);
    y[3] = MUL_C(COEF_CONST(2.9246284281582162), fs310);
    y[2] = MUL_C(COEF_CONST(4.0846110781292477), fs308);
    y[1] = MUL_C(COEF_CONST(6.7967507116736332), fs306);
    y[0] = MUL_R(REAL_CONST(20.3738781672314530), fs304);
}

#ifdef SBR_LOW_POWER

real_t fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9, fa10;
real_t fa11, fa12, fa13, fa14, fa15, fa16, fa17, fa18, fa19, fa20;
real_t fa21, fa22, fa23, fa24, fa25, fa26, fa27, fa28, fa31, fa32;
real_t fa33, fa34, fa37, fa38, fa39, fa40, fa41, fa42, fa43, fa44;
real_t fa45, fa46, fa47, fa48, fa49, fa51, fa53, fa54, fa57, fa58;
real_t fa59, fa60, fa61, fa62, fa63, fa64, fa65, fa66, fa67, fa68;
real_t fa69, fa70, fa71, fa72, fa73, fa74, fa75, fa76, fa77, fa78;
real_t fa79, fa80, fa81, fa82, fa83, fa84, fa85, fa86, fa87, fa88;
real_t fa89, fa90, fa91, fa92, fa95, fa96, fa97, fa98, fa101, fa102;
real_t fa103, fa104, fa107, fa108, fa109, fa110;
void DCT2_16_unscaled(real_t *y, real_t *x)
{
    fa0 = x[0] - x[15];
    fa1 = x[0] + x[15];
    fa2 = x[1] - x[14];
    fa3 = x[1] + x[14];
    fa4 = x[2] - x[13];
    fa5 = x[2] + x[13];
    fa6 = x[3] - x[12];
    fa7 = x[3] + x[12];
    fa8 = x[4] - x[11];
    fa9 = x[4] + x[11];
    fa10 = x[5] - x[10];
    fa11 = x[5] + x[10];
    fa12 = x[6] - x[9];
    fa13 = x[6] + x[9];
    fa14 = x[7] - x[8];
    fa15 = x[7] + x[8];
    fa16 = fa1 - fa15;
    fa17 = fa1 + fa15;
    fa18 = fa3 - fa13;
    fa19 = fa3 + fa13;
    fa20 = fa5 - fa11;
    fa21 = fa5 + fa11;
    fa22 = fa7 - fa9;
    fa23 = fa7 + fa9;
    fa24 = fa17 - fa23;
    fa25 = fa17 + fa23;
    fa26 = fa19 - fa21;
    fa27 = fa19 + fa21;
    fa28 = fa25 - fa27;
    y[0] = fa25 + fa27;
    y[8] = MUL_F(fa28, FRAC_CONST(0.7071067811865476));
    fa31 = fa24 + fa26;
    fa32 = MUL_C(fa24, COEF_CONST(1.3065629648763766));
    fa33 = MUL_F(fa31, FRAC_CONST(-0.9238795325112866));
    fa34 = MUL_F(fa26, FRAC_CONST(-0.5411961001461967));
    y[12] = fa32 + fa33;
    y[4] = fa34 - fa33;
    fa37 = fa16 + fa22;
    fa38 = MUL_C(fa16, COEF_CONST(1.1758756024193588));
    fa39 = MUL_F(fa37, FRAC_CONST(-0.9807852804032304));
    fa40 = MUL_F(fa22, FRAC_CONST(-0.7856949583871021));
    fa41 = fa38 + fa39;
    fa42 = fa40 - fa39;
    fa43 = fa18 + fa20;
    fa44 = MUL_C(fa18, COEF_CONST(1.3870398453221473));
    fa45 = MUL_F(fa43, FRAC_CONST(-0.8314696123025455));
    fa46 = MUL_F(fa20, FRAC_CONST(-0.2758993792829436));
    fa47 = fa44 + fa45;
    fa48 = fa46 - fa45;
    fa49 = fa42 - fa48;
    y[2] = fa42 + fa48;
    fa51 = MUL_F(fa49, FRAC_CONST(0.7071067811865476));
    y[14] = fa41 - fa47;
    fa53 = fa41 + fa47;
    fa54 = MUL_F(fa53, FRAC_CONST(0.7071067811865476));
    y[10] = fa51 - fa54;
    y[6] = fa51 + fa54;
    fa57 = fa2 - fa4;
    fa58 = fa2 + fa4;
    fa59 = fa6 - fa8;
    fa60 = fa6 + fa8;
    fa61 = fa10 - fa12;
    fa62 = fa10 + fa12;
    fa63 = MUL_F(fa60, FRAC_CONST(0.7071067811865476));
    fa64 = fa0 - fa63;
    fa65 = fa0 + fa63;
    fa66 = fa58 + fa62;
    fa67 = MUL_C(fa58, COEF_CONST(1.3065629648763766));
    fa68 = MUL_F(fa66, FRAC_CONST(-0.9238795325112866));
    fa69 = MUL_F(fa62, FRAC_CONST(-0.5411961001461967));
    fa70 = fa67 + fa68;
    fa71 = fa69 - fa68;
    fa72 = fa65 - fa71;
    fa73 = fa65 + fa71;
    fa74 = fa64 - fa70;
    fa75 = fa64 + fa70;
    fa76 = MUL_F(fa59, FRAC_CONST(0.7071067811865476));
    fa77 = fa14 - fa76;
    fa78 = fa14 + fa76;
    fa79 = fa61 + fa57;
    fa80 = MUL_C(fa61, COEF_CONST(1.3065629648763766));
    fa81 = MUL_F(fa79, FRAC_CONST(-0.9238795325112866));
    fa82 = MUL_F(fa57, FRAC_CONST(-0.5411961001461967));
    fa83 = fa80 + fa81;
    fa84 = fa82 - fa81;
    fa85 = fa78 - fa84;
    fa86 = fa78 + fa84;
    fa87 = fa77 - fa83;
    fa88 = fa77 + fa83;
    fa89 = fa86 + fa73;
    fa90 = MUL_F(fa86, FRAC_CONST(-0.8971675863426361));
    fa91 = MUL_F(fa89, FRAC_CONST(0.9951847266721968));
    fa92 = MUL_C(fa73, COEF_CONST(1.0932018670017576));
    y[1] = fa90 + fa91;
    y[15] = fa92 - fa91;
    fa95 = fa75 - fa88;
    fa96 = MUL_F(fa88, FRAC_CONST(-0.6666556584777466));
    fa97 = MUL_F(fa95, FRAC_CONST(0.9569403357322089));
    fa98 = MUL_C(fa75, COEF_CONST(1.2472250129866713));
    y[3] = fa97 - fa96;
    y[13] = fa98 - fa97;
    fa101 = fa87 + fa74;
    fa102 = MUL_F(fa87, FRAC_CONST(-0.4105245275223571));
    fa103 = MUL_F(fa101, FRAC_CONST(0.8819212643483549));
    fa104 = MUL_C(fa74, COEF_CONST(1.3533180011743529));
    y[5] = fa102 + fa103;
    y[11] = fa104 - fa103;
    fa107 = fa72 - fa85;
    fa108 = MUL_F(fa85, FRAC_CONST(-0.1386171691990915));
    fa109 = MUL_F(fa107, FRAC_CONST(0.7730104533627370));
    fa110 = MUL_C(fa72, COEF_CONST(1.4074037375263826));
    y[7] = fa109 - fa108;
    y[9] = fa110 - fa109;
}

real_t fb0, fb1, fb2, fb3, fb4, fb5, fb6, fb7, fb8, fb9, fb10;
real_t fb11, fb12, fb13, fb14, fb15, fb16, fb17, fb18, fb19, fb20;
real_t fb21, fb22, fb23, fb24, fb25, fb26, fb27, fb28, fb29, fb30;
real_t fb31, fb32, fb33, fb34, fb35, fb36, fb37, fb38, fb39, fb40;
real_t fb41, fb42, fb43, fb44, fb45, fb46, fb47, fb48, fb49, fb50;
real_t fb51, fb52, fb53, fb54, fb55, fb56, fb57, fb58, fb59, fb60;
real_t fb61, fb62, fb63, fb64, fb65, fb66, fb67, fb68, fb69, fb70;
real_t fb71, fb72, fb73, fb74, fb75, fb76, fb77, fb78, fb79, fb80;
real_t fb81, fb82, fb83, fb84, fb85, fb86, fb87, fb88, fb89, fb90;
real_t fb91, fb92, fb93, fb94, fb95, fb96, fb97, fb98, fb99, fb100;
real_t fb101, fb102, fb103, fb104, fb105, fb106, fb107, fb108, fb109, fb110;
real_t fb111, fb112, fb113, fb114, fb115, fb116, fb117, fb118, fb119, fb120;
real_t fb121, fb122, fb123, fb124, fb125, fb126, fb127, fb128, fb130, fb132;
real_t fb134, fb136, fb138, fb140, fb142, fb144, fb145, fb148, fb149, fb152;
real_t fb153, fb156, fb157;
void DCT4_16(real_t *y, real_t *x)
{
    fb0 = x[0] + x[15];
    fb1 = MUL_C(COEF_CONST(1.0478631305325901), x[0]);
    fb2 = MUL_F(FRAC_CONST(-0.9987954562051724), fb0);
    fb3 = MUL_F(FRAC_CONST(-0.9497277818777548), x[15]);
    fb4 = fb1 + fb2;
    fb5 = fb3 - fb2;
    fb6 = x[2] + x[13];
    fb7 = MUL_C(COEF_CONST(1.2130114330978077), x[2]);
    fb8 = MUL_F(FRAC_CONST(-0.9700312531945440), fb6);
    fb9 = MUL_F(FRAC_CONST(-0.7270510732912803), x[13]);
    fb10 = fb7 + fb8;
    fb11 = fb9 - fb8;
    fb12 = x[4] + x[11];
    fb13 = MUL_C(COEF_CONST(1.3315443865537255), x[4]);
    fb14 = MUL_F(FRAC_CONST(-0.9039892931234433), fb12);
    fb15 = MUL_F(FRAC_CONST(-0.4764341996931612), x[11]);
    fb16 = fb13 + fb14;
    fb17 = fb15 - fb14;
    fb18 = x[6] + x[9];
    fb19 = MUL_C(COEF_CONST(1.3989068359730781), x[6]);
    fb20 = MUL_F(FRAC_CONST(-0.8032075314806453), fb18);
    fb21 = MUL_F(FRAC_CONST(-0.2075082269882124), x[9]);
    fb22 = fb19 + fb20;
    fb23 = fb21 - fb20;
    fb24 = x[8] + x[7];
    fb25 = MUL_C(COEF_CONST(1.4125100802019777), x[8]);
    fb26 = MUL_F(FRAC_CONST(-0.6715589548470187), fb24);
    fb27 = MUL_F(FRAC_CONST(0.0693921705079402), x[7]);
    fb28 = fb25 + fb26;
    fb29 = fb27 - fb26;
    fb30 = x[10] + x[5];
    fb31 = MUL_C(COEF_CONST(1.3718313541934939), x[10]);
    fb32 = MUL_F(FRAC_CONST(-0.5141027441932219), fb30);
    fb33 = MUL_F(FRAC_CONST(0.3436258658070501), x[5]);
    fb34 = fb31 + fb32;
    fb35 = fb33 - fb32;
    fb36 = x[12] + x[3];
    fb37 = MUL_C(COEF_CONST(1.2784339185752409), x[12]);
    fb38 = MUL_F(FRAC_CONST(-0.3368898533922200), fb36);
    fb39 = MUL_F(FRAC_CONST(0.6046542117908008), x[3]);
    fb40 = fb37 + fb38;
    fb41 = fb39 - fb38;
    fb42 = x[14] + x[1];
    fb43 = MUL_C(COEF_CONST(1.1359069844201433), x[14]);
    fb44 = MUL_F(FRAC_CONST(-0.1467304744553624), fb42);
    fb45 = MUL_F(FRAC_CONST(0.8424460355094185), x[1]);
    fb46 = fb43 + fb44;
    fb47 = fb45 - fb44;
    fb48 = fb5 - fb29;
    fb49 = fb5 + fb29;
    fb50 = fb4 - fb28;
    fb51 = fb4 + fb28;
    fb52 = fb11 - fb35;
    fb53 = fb11 + fb35;
    fb54 = fb10 - fb34;
    fb55 = fb10 + fb34;
    fb56 = fb17 - fb41;
    fb57 = fb17 + fb41;
    fb58 = fb16 - fb40;
    fb59 = fb16 + fb40;
    fb60 = fb23 - fb47;
    fb61 = fb23 + fb47;
    fb62 = fb22 - fb46;
    fb63 = fb22 + fb46;
    fb64 = fb48 + fb50;
    fb65 = MUL_C(COEF_CONST(1.1758756024193588), fb48);
    fb66 = MUL_F(FRAC_CONST(-0.9807852804032304), fb64);
    fb67 = MUL_F(FRAC_CONST(-0.7856949583871021), fb50);
    fb68 = fb65 + fb66;
    fb69 = fb67 - fb66;
    fb70 = fb52 + fb54;
    fb71 = MUL_C(COEF_CONST(1.3870398453221475), fb52);
    fb72 = MUL_F(FRAC_CONST(-0.5555702330196022), fb70);
    fb73 = MUL_F(FRAC_CONST(0.2758993792829431), fb54);
    fb74 = fb71 + fb72;
    fb75 = fb73 - fb72;
    fb76 = fb56 + fb58;
    fb77 = MUL_F(FRAC_CONST(0.7856949583871022), fb56);
    fb78 = MUL_F(FRAC_CONST(0.1950903220161283), fb76);
    fb79 = MUL_C(COEF_CONST(1.1758756024193586), fb58);
    fb80 = fb77 + fb78;
    fb81 = fb79 - fb78;
    fb82 = fb60 + fb62;
    fb83 = MUL_F(FRAC_CONST(-0.2758993792829430), fb60);
    fb84 = MUL_F(FRAC_CONST(0.8314696123025452), fb82);
    fb85 = MUL_C(COEF_CONST(1.3870398453221475), fb62);
    fb86 = fb83 + fb84;
    fb87 = fb85 - fb84;
    fb88 = fb49 - fb57;
    fb89 = fb49 + fb57;
    fb90 = fb51 - fb59;
    fb91 = fb51 + fb59;
    fb92 = fb53 - fb61;
    fb93 = fb53 + fb61;
    fb94 = fb55 - fb63;
    fb95 = fb55 + fb63;
    fb96 = fb69 - fb81;
    fb97 = fb69 + fb81;
    fb98 = fb68 - fb80;
    fb99 = fb68 + fb80;
    fb100 = fb75 - fb87;
    fb101 = fb75 + fb87;
    fb102 = fb74 - fb86;
    fb103 = fb74 + fb86;
    fb104 = fb88 + fb90;
    fb105 = MUL_C(COEF_CONST(1.3065629648763766), fb88);
    fb106 = MUL_F(FRAC_CONST(-0.9238795325112866), fb104);
    fb107 = MUL_F(FRAC_CONST(-0.5411961001461967), fb90);
    fb108 = fb105 + fb106;
    fb109 = fb107 - fb106;
    fb110 = fb92 + fb94;
    fb111 = MUL_F(FRAC_CONST(0.5411961001461969), fb92);
    fb112 = MUL_F(FRAC_CONST(0.3826834323650898), fb110);
    fb113 = MUL_C(COEF_CONST(1.3065629648763766), fb94);
    fb114 = fb111 + fb112;
    fb115 = fb113 - fb112;
    fb116 = fb96 + fb98;
    fb117 = MUL_C(COEF_CONST(1.3065629648763766), fb96);
    fb118 = MUL_F(FRAC_CONST(-0.9238795325112866), fb116);
    fb119 = MUL_F(FRAC_CONST(-0.5411961001461967), fb98);
    fb120 = fb117 + fb118;
    fb121 = fb119 - fb118;
    fb122 = fb100 + fb102;
    fb123 = MUL_F(FRAC_CONST(0.5411961001461969), fb100);
    fb124 = MUL_F(FRAC_CONST(0.3826834323650898), fb122);
    fb125 = MUL_C(COEF_CONST(1.3065629648763766), fb102);
    fb126 = fb123 + fb124;
    fb127 = fb125 - fb124;
    fb128 = fb89 - fb93;
    y[0] = fb89 + fb93;
    fb130 = fb91 - fb95;
    y[15] = fb91 + fb95;
    fb132 = fb109 - fb115;
    y[3] = fb109 + fb115;
    fb134 = fb108 - fb114;
    y[12] = fb108 + fb114;
    fb136 = fb97 - fb101;
    y[1] = fb97 + fb101;
    fb138 = fb99 - fb103;
    y[14] = fb99 + fb103;
    fb140 = fb121 - fb127;
    y[2] = fb121 + fb127;
    fb142 = fb120 - fb126;
    y[13] = fb20 + fb126;
    fb144 = fb128 - fb130;
    fb145 = fb128 + fb130;
    y[8] = MUL_F(FRAC_CONST(0.7071067811865474), fb144);
    y[7] = MUL_F(FRAC_CONST(0.7071067811865474), fb145);
    fb148 = fb132 - fb134;
    fb149 = fb132 + fb134;
    y[11] = MUL_F(FRAC_CONST(0.7071067811865474), fb148);
    y[4] = MUL_F(FRAC_CONST(0.7071067811865474), fb149);
    fb152 = fb136 - fb138;
    fb153 = fb136 + fb138;
    y[9] = MUL_F(FRAC_CONST(0.7071067811865474), fb152);
    y[6] = MUL_F(FRAC_CONST(0.7071067811865474), fb153);
    fb156 = fb140 - fb142;
    fb157 = fb140 + fb142;
    y[10] = MUL_F(FRAC_CONST(0.7071067811865474), fb156);
    y[5] = MUL_F(FRAC_CONST(0.7071067811865474), fb157);
}

real_t fe0, fe1, fe2, fe3, fe4, fe5, fe6, fe7, fe8, fe9, fe10;
real_t fe11, fe12, fe13, fe14, fe15, fe16, fe17, fe18, fe19, fe20;
real_t fe21, fe22, fe23, fe24, fe25, fe26, fe27, fe28, fe29, fe30;
real_t fe31, fe32, fe33, fe34, fe35, fe36, fe37, fe38, fe39, fe40;
real_t fe41, fe42, fe43, fe44, fe45, fe46, fe47, fe48, fe49, fe50;
real_t fe51, fe52, fe53, fe54, fe55, fe56, fe57, fe58, fe59, fe60;
real_t fe61, fe62, fe63, fe64, fe65, fe66, fe67, fe68, fe69, fe70;
real_t fe71, fe72, fe73, fe74, fe75, fe76, fe77, fe78, fe79, fe80;
real_t fe81, fe82, fe83, fe84, fe85, fe86, fe87, fe88, fe89, fe90;
real_t fe91, fe92, fe93, fe94, fe95, fe96, fe97, fe98, fe99, fe100;
real_t fe101, fe102, fe103, fe104, fe105, fe106, fe107, fe108, fe109, fe110;
real_t fe111, fe112, fe113, fe114, fe115, fe116, fe117, fe118, fe119, fe120;
real_t fe121, fe122, fe123, fe124, fe125, fe126, fe127, fe128, fe129, fe130;
real_t fe131, fe132, fe133, fe134, fe135, fe136, fe137, fe138, fe139, fe140;
real_t fe141, fe142, fe143, fe144, fe145, fe146, fe147, fe148, fe149, fe150;
real_t fe151, fe152, fe153, fe154, fe155, fe156, fe157, fe158, fe159, fe160;
real_t fe161, fe162, fe163, fe164, fe165, fe166, fe167, fe168, fe169, fe170;
real_t fe171, fe172, fe173, fe174, fe175, fe176, fe177, fe178, fe179, fe180;
real_t fe181, fe182, fe183, fe184, fe185, fe186, fe187, fe188, fe189, fe190;
real_t fe191, fe192, fe193, fe194, fe195, fe196, fe197, fe198, fe199, fe200;
real_t fe201, fe202, fe203, fe204, fe205, fe206, fe207, fe208, fe209, fe210;
real_t fe211, fe212, fe213, fe214, fe215, fe216, fe217, fe218, fe219, fe220;
real_t fe221, fe222, fe223, fe224, fe225, fe226, fe227, fe228, fe229, fe230;
real_t fe231, fe232, fe233, fe234, fe235, fe236, fe237, fe238, fe239, fe240;
real_t fe241, fe242, fe243, fe244, fe245, fe246, fe247, fe248, fe249, fe250;
real_t fe251, fe252, fe253, fe254, fe255, fe256, fe257, fe258, fe259, fe260;
real_t fe261, fe262, fe263, fe264, fe265, fe266, fe267, fe268, fe269, fe270;
real_t fe271, fe272;
void DCT3_32_unscaled(real_t *y, real_t *x)
{
    fe0 = MUL_F(x[16], FRAC_CONST(0.7071067811865476));
    fe1 = x[0] - fe0;
    fe2 = x[0] + fe0;
    fe3 = x[8] + x[24];
    fe4 = MUL_C(x[8], COEF_CONST(1.3065629648763766));
    fe5 = MUL_F(fe3, FRAC_CONST((-0.9238795325112866)));
    fe6 = MUL_F(x[24], FRAC_CONST((-0.5411961001461967)));
    fe7 = fe4 + fe5;
    fe8 = fe6 - fe5;
    fe9 = fe2 - fe8;
    fe10 = fe2 + fe8;
    fe11 = fe1 - fe7;
    fe12 = fe1 + fe7;
    fe13 = x[4] + x[28];
    fe14 = MUL_C(x[4], COEF_CONST(1.1758756024193588));
    fe15 = MUL_F(fe13, FRAC_CONST((-0.9807852804032304)));
    fe16 = MUL_F(x[28], FRAC_CONST((-0.7856949583871021)));
    fe17 = fe14 + fe15;
    fe18 = fe16 - fe15;
    fe19 = x[12] + x[20];
    fe20 = MUL_C(x[12], COEF_CONST(1.3870398453221473));
    fe21 = MUL_F(fe19, FRAC_CONST((-0.8314696123025455)));
    fe22 = MUL_F(x[20], FRAC_CONST((-0.2758993792829436)));
    fe23 = fe20 + fe21;
    fe24 = fe22 - fe21;
    fe25 = fe18 - fe24;
    fe26 = fe18 + fe24;
    fe27 = MUL_F(fe25, FRAC_CONST(0.7071067811865476));
    fe28 = fe17 - fe23;
    fe29 = fe17 + fe23;
    fe30 = MUL_F(fe29, FRAC_CONST(0.7071067811865476));
    fe31 = fe27 - fe30;
    fe32 = fe27 + fe30;
    fe33 = fe10 - fe26;
    fe34 = fe10 + fe26;
    fe35 = fe12 - fe32;
    fe36 = fe12 + fe32;
    fe37 = fe11 - fe31;
    fe38 = fe11 + fe31;
    fe39 = fe9 - fe28;
    fe40 = fe9 + fe28;
    fe41 = x[2] + x[30];
    fe42 = MUL_C(x[2], COEF_CONST(1.0932018670017569));
    fe43 = MUL_F(fe41, FRAC_CONST((-0.9951847266721969)));
    fe44 = MUL_F(x[30], FRAC_CONST((-0.8971675863426368)));
    fe45 = fe42 + fe43;
    fe46 = fe44 - fe43;
    fe47 = x[6] + x[26];
    fe48 = MUL_C(x[6], COEF_CONST(1.2472250129866711));
    fe49 = MUL_F(fe47, FRAC_CONST((-0.9569403357322089)));
    fe50 = MUL_F(x[26], FRAC_CONST((-0.6666556584777469)));
    fe51 = fe48 + fe49;
    fe52 = fe50 - fe49;
    fe53 = x[10] + x[22];
    fe54 = MUL_C(x[10], COEF_CONST(1.3533180011743526));
    fe55 = MUL_F(fe53, FRAC_CONST((-0.8819212643483551)));
    fe56 = MUL_F(x[22], FRAC_CONST((-0.4105245275223575)));
    fe57 = fe54 + fe55;
    fe58 = fe56 - fe55;
    fe59 = x[14] + x[18];
    fe60 = MUL_C(x[14], COEF_CONST(1.4074037375263826));
    fe61 = MUL_F(fe59, FRAC_CONST((-0.7730104533627369)));
    fe62 = MUL_F(x[18], FRAC_CONST((-0.1386171691990913)));
    fe63 = fe60 + fe61;
    fe64 = fe62 - fe61;
    fe65 = fe46 - fe64;
    fe66 = fe46 + fe64;
    fe67 = fe52 - fe58;
    fe68 = fe52 + fe58;
    fe69 = fe66 - fe68;
    fe70 = fe66 + fe68;
    fe71 = MUL_F(fe69, FRAC_CONST(0.7071067811865476));
    fe72 = fe65 + fe67;
    fe73 = MUL_C(fe65, COEF_CONST(1.3065629648763766));
    fe74 = MUL_F(fe72, FRAC_CONST((-0.9238795325112866)));
    fe75 = MUL_F(fe67, FRAC_CONST((-0.5411961001461967)));
    fe76 = fe73 + fe74;
    fe77 = fe75 - fe74;
    fe78 = fe45 - fe63;
    fe79 = fe45 + fe63;
    fe80 = fe51 - fe57;
    fe81 = fe51 + fe57;
    fe82 = fe79 + fe81;
    fe83 = MUL_C(fe79, COEF_CONST(1.3065629648763770));
    fe84 = MUL_F(fe82, FRAC_CONST((-0.3826834323650904)));
    fe85 = MUL_F(fe81, FRAC_CONST(0.5411961001461961));
    fe86 = fe83 + fe84;
    fe87 = fe85 - fe84;
    fe88 = fe78 - fe80;
    fe89 = fe78 + fe80;
    fe90 = MUL_F(fe89, FRAC_CONST(0.7071067811865476));
    fe91 = fe77 - fe87;
    fe92 = fe77 + fe87;
    fe93 = fe71 - fe90;
    fe94 = fe71 + fe90;
    fe95 = fe76 - fe86;
    fe96 = fe76 + fe86;
    fe97 = fe34 - fe70;
    fe98 = fe34 + fe70;
    fe99 = fe36 - fe92;
    fe100 = fe36 + fe92;
    fe101 = fe38 - fe91;
    fe102 = fe38 + fe91;
    fe103 = fe40 - fe94;
    fe104 = fe40 + fe94;
    fe105 = fe39 - fe93;
    fe106 = fe39 + fe93;
    fe107 = fe37 - fe96;
    fe108 = fe37 + fe96;
    fe109 = fe35 - fe95;
    fe110 = fe35 + fe95;
    fe111 = fe33 - fe88;
    fe112 = fe33 + fe88;
    fe113 = x[1] + x[31];
    fe114 = MUL_C(x[1], COEF_CONST(1.0478631305325901));
    fe115 = MUL_F(fe113, FRAC_CONST((-0.9987954562051724)));
    fe116 = MUL_F(x[31], FRAC_CONST((-0.9497277818777548)));
    fe117 = fe114 + fe115;
    fe118 = fe116 - fe115;
    fe119 = x[5] + x[27];
    fe120 = MUL_C(x[5], COEF_CONST(1.2130114330978077));
    fe121 = MUL_F(fe119, FRAC_CONST((-0.9700312531945440)));
    fe122 = MUL_F(x[27], FRAC_CONST((-0.7270510732912803)));
    fe123 = fe120 + fe121;
    fe124 = fe122 - fe121;
    fe125 = x[9] + x[23];
    fe126 = MUL_C(x[9], COEF_CONST(1.3315443865537255));
    fe127 = MUL_F(fe125, FRAC_CONST((-0.9039892931234433)));
    fe128 = MUL_F(x[23], FRAC_CONST((-0.4764341996931612)));
    fe129 = fe126 + fe127;
    fe130 = fe128 - fe127;
    fe131 = x[13] + x[19];
    fe132 = MUL_C(x[13], COEF_CONST(1.3989068359730781));
    fe133 = MUL_F(fe131, FRAC_CONST((-0.8032075314806453)));
    fe134 = MUL_F(x[19], FRAC_CONST((-0.2075082269882124)));
    fe135 = fe132 + fe133;
    fe136 = fe134 - fe133;
    fe137 = x[17] + x[15];
    fe138 = MUL_C(x[17], COEF_CONST(1.4125100802019777));
    fe139 = MUL_F(fe137, FRAC_CONST((-0.6715589548470187)));
    fe140 = MUL_F(x[15], FRAC_CONST(0.0693921705079402));
    fe141 = fe138 + fe139;
    fe142 = fe140 - fe139;
    fe143 = x[21] + x[11];
    fe144 = MUL_C(x[21], COEF_CONST(1.3718313541934939));
    fe145 = MUL_F(fe143, FRAC_CONST((-0.5141027441932219)));
    fe146 = MUL_F(x[11], FRAC_CONST(0.3436258658070501));
    fe147 = fe144 + fe145;
    fe148 = fe146 - fe145;
    fe149 = x[25] + x[7];
    fe150 = MUL_C(x[25], COEF_CONST(1.2784339185752409));
    fe151 = MUL_F(fe149, FRAC_CONST((-0.3368898533922200)));
    fe152 = MUL_F(x[7], FRAC_CONST(0.6046542117908008));
    fe153 = fe150 + fe151;
    fe154 = fe152 - fe151;
    fe155 = x[29] + x[3];
    fe156 = MUL_C(x[29], COEF_CONST(1.1359069844201433));
    fe157 = MUL_F(fe155, FRAC_CONST((-0.1467304744553624)));
    fe158 = MUL_F(x[3], FRAC_CONST(0.8424460355094185));
    fe159 = fe156 + fe157;
    fe160 = fe158 - fe157;
    fe161 = fe118 - fe142;
    fe162 = fe118 + fe142;
    fe163 = fe117 - fe141;
    fe164 = fe117 + fe141;
    fe165 = fe124 - fe148;
    fe166 = fe124 + fe148;
    fe167 = fe123 - fe147;
    fe168 = fe123 + fe147;
    fe169 = fe130 - fe154;
    fe170 = fe130 + fe154;
    fe171 = fe129 - fe153;
    fe172 = fe129 + fe153;
    fe173 = fe136 - fe160;
    fe174 = fe136 + fe160;
    fe175 = fe135 - fe159;
    fe176 = fe135 + fe159;
    fe177 = fe161 + fe163;
    fe178 = MUL_C(fe161, COEF_CONST(1.1758756024193588));
    fe179 = MUL_F(fe177, FRAC_CONST((-0.9807852804032304)));
    fe180 = MUL_F(fe163, FRAC_CONST((-0.7856949583871021)));
    fe181 = fe178 + fe179;
    fe182 = fe180 - fe179;
    fe183 = fe165 + fe167;
    fe184 = MUL_C(fe165, COEF_CONST(1.3870398453221475));
    fe185 = MUL_F(fe183, FRAC_CONST((-0.5555702330196022)));
    fe186 = MUL_F(fe167, FRAC_CONST(0.2758993792829431));
    fe187 = fe184 + fe185;
    fe188 = fe186 - fe185;
    fe189 = fe169 + fe171;
    fe190 = MUL_F(fe169, FRAC_CONST(0.7856949583871022));
    fe191 = MUL_F(fe189, FRAC_CONST(0.1950903220161283));
    fe192 = MUL_C(fe171, COEF_CONST(1.1758756024193586));
    fe193 = fe190 + fe191;
    fe194 = fe192 - fe191;
    fe195 = fe173 + fe175;
    fe196 = MUL_F(fe173, FRAC_CONST((-0.2758993792829430)));
    fe197 = MUL_F(fe195, FRAC_CONST(0.8314696123025452));
    fe198 = MUL_C(fe175, COEF_CONST(1.3870398453221475));
    fe199 = fe196 + fe197;
    fe200 = fe198 - fe197;
    fe201 = fe162 - fe170;
    fe202 = fe162 + fe170;
    fe203 = fe164 - fe172;
    fe204 = fe164 + fe172;
    fe205 = fe166 - fe174;
    fe206 = fe166 + fe174;
    fe207 = fe168 - fe176;
    fe208 = fe168 + fe176;
    fe209 = fe182 - fe194;
    fe210 = fe182 + fe194;
    fe211 = fe181 - fe193;
    fe212 = fe181 + fe193;
    fe213 = fe188 - fe200;
    fe214 = fe188 + fe200;
    fe215 = fe187 - fe199;
    fe216 = fe187 + fe199;
    fe217 = fe201 + fe203;
    fe218 = MUL_C(fe201, COEF_CONST(1.3065629648763766));
    fe219 = MUL_F(fe217, FRAC_CONST((-0.9238795325112866)));
    fe220 = MUL_F(fe203, FRAC_CONST((-0.5411961001461967)));
    fe221 = fe218 + fe219;
    fe222 = fe220 - fe219;
    fe223 = fe205 + fe207;
    fe224 = MUL_F(fe205, FRAC_CONST(0.5411961001461969));
    fe225 = MUL_F(fe223, FRAC_CONST(0.3826834323650898));
    fe226 = MUL_C(fe207, COEF_CONST(1.3065629648763766));
    fe227 = fe224 + fe225;
    fe228 = fe226 - fe225;
    fe229 = fe209 + fe211;
    fe230 = MUL_C(fe209, COEF_CONST(1.3065629648763766));
    fe231 = MUL_F(fe229, FRAC_CONST((-0.9238795325112866)));
    fe232 = MUL_F(fe211, FRAC_CONST((-0.5411961001461967)));
    fe233 = fe230 + fe231;
    fe234 = fe232 - fe231;
    fe235 = fe213 + fe215;
    fe236 = MUL_F(fe213, FRAC_CONST(0.5411961001461969));
    fe237 = MUL_F(fe235, FRAC_CONST(0.3826834323650898));
    fe238 = MUL_C(fe215, COEF_CONST(1.3065629648763766));
    fe239 = fe236 + fe237;
    fe240 = fe238 - fe237;
    fe241 = fe202 - fe206;
    fe242 = fe202 + fe206;
    fe243 = fe204 - fe208;
    fe244 = fe204 + fe208;
    fe245 = fe222 - fe228;
    fe246 = fe222 + fe228;
    fe247 = fe221 - fe227;
    fe248 = fe221 + fe227;
    fe249 = fe210 - fe214;
    fe250 = fe210 + fe214;
    fe251 = fe212 - fe216;
    fe252 = fe212 + fe216;
    fe253 = fe234 - fe240;
    fe254 = fe234 + fe240;
    fe255 = fe233 - fe239;
    fe256 = fe233 + fe239;
    fe257 = fe241 - fe243;
    fe258 = fe241 + fe243;
    fe259 = MUL_F(fe257, FRAC_CONST(0.7071067811865474));
    fe260 = MUL_F(fe258, FRAC_CONST(0.7071067811865474));
    fe261 = fe245 - fe247;
    fe262 = fe245 + fe247;
    fe263 = MUL_F(fe261, FRAC_CONST(0.7071067811865474));
    fe264 = MUL_F(fe262, FRAC_CONST(0.7071067811865474));
    fe265 = fe249 - fe251;
    fe266 = fe249 + fe251;
    fe267 = MUL_F(fe265, FRAC_CONST(0.7071067811865474));
    fe268 = MUL_F(fe266, FRAC_CONST(0.7071067811865474));
    fe269 = fe253 - fe255;
    fe270 = fe253 + fe255;
    fe271 = MUL_F(fe269, FRAC_CONST(0.7071067811865474));
    fe272 = MUL_F(fe270, FRAC_CONST(0.7071067811865474));
    y[31] = fe98 - fe242;
    y[0] = fe98 + fe242;
    y[30] = fe100 - fe250;
    y[1] = fe100 + fe250;
    y[29] = fe102 - fe254;
    y[2] = fe102 + fe254;
    y[28] = fe104 - fe246;
    y[3] = fe104 + fe246;
    y[27] = fe106 - fe264;
    y[4] = fe106 + fe264;
    y[26] = fe108 - fe272;
    y[5] = fe108 + fe272;
    y[25] = fe110 - fe268;
    y[6] = fe110 + fe268;
    y[24] = fe112 - fe260;
    y[7] = fe112 + fe260;
    y[23] = fe111 - fe259;
    y[8] = fe111 + fe259;
    y[22] = fe109 - fe267;
    y[9] = fe109 + fe267;
    y[21] = fe107 - fe271;
    y[10] = fe107 + fe271;
    y[20] = fe105 - fe263;
    y[11] = fe105 + fe263;
    y[19] = fe103 - fe248;
    y[12] = fe103 + fe248;
    y[18] = fe101 - fe256;
    y[13] = fe101 + fe256;
    y[17] = fe99 - fe252;
    y[14] = fe99 + fe252;
    y[16] = fe97 - fe244;
    y[15] = fe97 + fe244;
}

real_t fd0, fd1, fd2, fd3, fd4, fd5, fd6, fd7, fd8, fd9, fd10;
real_t fd11, fd12, fd13, fd14, fd15, fd16, fd17, fd18, fd19, fd20;
real_t fd21, fd22, fd23, fd24, fd25, fd26, fd27, fd28, fd29, fd30;
real_t fd31, fd32, fd33, fd34, fd35, fd36, fd37, fd38, fd39, fd40;
real_t fd41, fd42, fd43, fd44, fd45, fd46, fd47, fd48, fd49, fd50;
real_t fd51, fd52, fd53, fd54, fd55, fd56, fd57, fd58, fd59, fd60;
real_t fd63, fd64, fd65, fd66, fd69, fd70, fd71, fd72, fd73, fd74;
real_t fd75, fd76, fd77, fd78, fd79, fd80, fd81, fd83, fd85, fd86;
real_t fd89, fd90, fd91, fd92, fd93, fd94, fd95, fd96, fd97, fd98;
real_t fd99, fd100, fd101, fd102, fd103, fd104, fd105, fd106, fd107, fd108;
real_t fd109, fd110, fd111, fd112, fd113, fd114, fd115, fd116, fd117, fd118;
real_t fd119, fd120, fd121, fd122, fd123, fd124, fd127, fd128, fd129, fd130;
real_t fd133, fd134, fd135, fd136, fd139, fd140, fd141, fd142, fd145, fd146;
real_t fd147, fd148, fd149, fd150, fd151, fd152, fd153, fd154, fd155, fd156;
real_t fd157, fd158, fd159, fd160, fd161, fd162, fd163, fd164, fd165, fd166;
real_t fd167, fd168, fd169, fd170, fd171, fd172, fd173, fd174, fd175, fd176;
real_t fd177, fd178, fd179, fd180, fd181, fd182, fd183, fd184, fd185, fd186;
real_t fd187, fd188, fd189, fd190, fd191, fd192, fd193, fd194, fd195, fd196;
real_t fd197, fd198, fd199, fd200, fd201, fd202, fd203, fd204, fd205, fd206;
real_t fd207, fd208, fd209, fd210, fd211, fd212, fd213, fd214, fd215, fd216;
real_t fd217, fd218, fd219, fd220, fd221, fd222, fd223, fd224, fd225, fd226;
real_t fd227, fd228, fd229, fd230, fd231, fd232, fd233, fd234, fd235, fd236;
real_t fd237, fd238, fd239, fd240, fd241, fd242, fd243, fd244, fd247, fd248;
real_t fd249, fd250, fd253, fd254, fd255, fd256, fd259, fd260, fd261, fd262;
real_t fd265, fd266, fd267, fd268, fd271, fd272, fd273, fd274, fd277, fd278;
real_t fd279, fd280, fd283, fd284, fd285, fd286;

void DCT2_32_unscaled(real_t *y, real_t *x)
{
    fd0 = x[0] - x[31];
    fd1 = x[0] + x[31];
    fd2 = x[1] - x[30];
    fd3 = x[1] + x[30];
    fd4 = x[2] - x[29];
    fd5 = x[2] + x[29];
    fd6 = x[3] - x[28];
    fd7 = x[3] + x[28];
    fd8 = x[4] - x[27];
    fd9 = x[4] + x[27];
    fd10 = x[5] - x[26];
    fd11 = x[5] + x[26];
    fd12 = x[6] - x[25];
    fd13 = x[6] + x[25];
    fd14 = x[7] - x[24];
    fd15 = x[7] + x[24];
    fd16 = x[8] - x[23];
    fd17 = x[8] + x[23];
    fd18 = x[9] - x[22];
    fd19 = x[9] + x[22];
    fd20 = x[10] - x[21];
    fd21 = x[10] + x[21];
    fd22 = x[11] - x[20];
    fd23 = x[11] + x[20];
    fd24 = x[12] - x[19];
    fd25 = x[12] + x[19];
    fd26 = x[13] - x[18];
    fd27 = x[13] + x[18];
    fd28 = x[14] - x[17];
    fd29 = x[14] + x[17];
    fd30 = x[15] - x[16];
    fd31 = x[15] + x[16];
    fd32 = fd1 - fd31;
    fd33 = fd1 + fd31;
    fd34 = fd3 - fd29;
    fd35 = fd3 + fd29;
    fd36 = fd5 - fd27;
    fd37 = fd5 + fd27;
    fd38 = fd7 - fd25;
    fd39 = fd7 + fd25;
    fd40 = fd9 - fd23;
    fd41 = fd9 + fd23;
    fd42 = fd11 - fd21;
    fd43 = fd11 + fd21;
    fd44 = fd13 - fd19;
    fd45 = fd13 + fd19;
    fd46 = fd15 - fd17;
    fd47 = fd15 + fd17;
    fd48 = fd33 - fd47;
    fd49 = fd33 + fd47;
    fd50 = fd35 - fd45;
    fd51 = fd35 + fd45;
    fd52 = fd37 - fd43;
    fd53 = fd37 + fd43;
    fd54 = fd39 - fd41;
    fd55 = fd39 + fd41;
    fd56 = fd49 - fd55;
    fd57 = fd49 + fd55;
    fd58 = fd51 - fd53;
    fd59 = fd51 + fd53;
    fd60 = fd57 - fd59;
    y[0] = fd57 + fd59;
    y[16] = MUL_F(FRAC_CONST(0.7071067811865476), fd60);
    fd63 = fd56 + fd58;
    fd64 = MUL_C(COEF_CONST(1.3065629648763766), fd56);
    fd65 = MUL_F(FRAC_CONST(-0.9238795325112866), fd63);
    fd66 = MUL_F(FRAC_CONST(-0.5411961001461967), fd58);
    y[24] = fd64 + fd65;
    y[8] = fd66 - fd65;
    fd69 = fd48 + fd54;
    fd70 = MUL_C(COEF_CONST(1.1758756024193588), fd48);
    fd71 = MUL_F(FRAC_CONST(-0.9807852804032304), fd69);
    fd72 = MUL_F(FRAC_CONST(-0.7856949583871021), fd54);
    fd73 = fd70 + fd71;
    fd74 = fd72 - fd71;
    fd75 = fd50 + fd52;
    fd76 = MUL_C(COEF_CONST(1.3870398453221473), fd50);
    fd77 = MUL_F(FRAC_CONST(-0.8314696123025455), fd75);
    fd78 = MUL_F(FRAC_CONST(-0.2758993792829436), fd52);
    fd79 = fd76 + fd77;
    fd80 = fd78 - fd77;
    fd81 = fd74 - fd80;
    y[4] = fd74 + fd80;
    fd83 = MUL_F(FRAC_CONST(0.7071067811865476), fd81);
    y[28] = fd73 - fd79;
    fd85 = fd73 + fd79;
    fd86 = MUL_F(FRAC_CONST(0.7071067811865476), fd85);
    y[20] = fd83 - fd86;
    y[12] = fd83 + fd86;
    fd89 = fd34 - fd36;
    fd90 = fd34 + fd36;
    fd91 = fd38 - fd40;
    fd92 = fd38 + fd40;
    fd93 = fd42 - fd44;
    fd94 = fd42 + fd44;
    fd95 = MUL_F(FRAC_CONST(0.7071067811865476), fd92);
    fd96 = fd32 - fd95;
    fd97 = fd32 + fd95;
    fd98 = fd90 + fd94;
    fd99 = MUL_C(COEF_CONST(1.3065629648763766), fd90);
    fd100 = MUL_F(FRAC_CONST(-0.9238795325112866), fd98);
    fd101 = MUL_F(FRAC_CONST(-0.5411961001461967), fd94);
    fd102 = fd99 + fd100;
    fd103 = fd101 - fd100;
    fd104 = fd97 - fd103;
    fd105 = fd97 + fd103;
    fd106 = fd96 - fd102;
    fd107 = fd96 + fd102;
    fd108 = MUL_F(FRAC_CONST(0.7071067811865476), fd91);
    fd109 = fd46 - fd108;
    fd110 = fd46 + fd108;
    fd111 = fd93 + fd89;
    fd112 = MUL_C(COEF_CONST(1.3065629648763766), fd93);
    fd113 = MUL_F(FRAC_CONST(-0.9238795325112866), fd111);
    fd114 = MUL_F(FRAC_CONST(-0.5411961001461967), fd89);
    fd115 = fd112 + fd113;
    fd116 = fd114 - fd113;
    fd117 = fd110 - fd116;
    fd118 = fd110 + fd116;
    fd119 = fd109 - fd115;
    fd120 = fd109 + fd115;
    fd121 = fd118 + fd105;
    fd122 = MUL_F(FRAC_CONST(-0.8971675863426361), fd118);
    fd123 = MUL_F(FRAC_CONST(0.9951847266721968), fd121);
    fd124 = MUL_C(COEF_CONST(1.0932018670017576), fd105);
    y[2] = fd122 + fd123;
    y[30] = fd124 - fd123;
    fd127 = fd107 - fd120;
    fd128 = MUL_F(FRAC_CONST(-0.6666556584777466), fd120);
    fd129 = MUL_F(FRAC_CONST(0.9569403357322089), fd127);
    fd130 = MUL_C(COEF_CONST(1.2472250129866713), fd107);
    y[6] = fd129 - fd128;
    y[26] = fd130 - fd129;
    fd133 = fd119 + fd106;
    fd134 = MUL_F(FRAC_CONST(-0.4105245275223571), fd119);
    fd135 = MUL_F(FRAC_CONST(0.8819212643483549), fd133);
    fd136 = MUL_C(COEF_CONST(1.3533180011743529), fd106);
    y[10] = fd134 + fd135;
    y[22] = fd136 - fd135;
    fd139 = fd104 - fd117;
    fd140 = MUL_F(FRAC_CONST(-0.1386171691990915), fd117);
    fd141 = MUL_F(FRAC_CONST(0.7730104533627370), fd139);
    fd142 = MUL_C(COEF_CONST(1.4074037375263826), fd104);
    y[14] = fd141 - fd140;
    y[18] = fd142 - fd141;
    fd145 = fd2 - fd4;
    fd146 = fd2 + fd4;
    fd147 = fd6 - fd8;
    fd148 = fd6 + fd8;
    fd149 = fd10 - fd12;
    fd150 = fd10 + fd12;
    fd151 = fd14 - fd16;
    fd152 = fd14 + fd16;
    fd153 = fd18 - fd20;
    fd154 = fd18 + fd20;
    fd155 = fd22 - fd24;
    fd156 = fd22 + fd24;
    fd157 = fd26 - fd28;
    fd158 = fd26 + fd28;
    fd159 = MUL_F(FRAC_CONST(0.7071067811865476), fd152);
    fd160 = fd0 - fd159;
    fd161 = fd0 + fd159;
    fd162 = fd148 + fd156;
    fd163 = MUL_C(COEF_CONST(1.3065629648763766), fd148);
    fd164 = MUL_F(FRAC_CONST(-0.9238795325112866), fd162);
    fd165 = MUL_F(FRAC_CONST(-0.5411961001461967), fd156);
    fd166 = fd163 + fd164;
    fd167 = fd165 - fd164;
    fd168 = fd161 - fd167;
    fd169 = fd161 + fd167;
    fd170 = fd160 - fd166;
    fd171 = fd160 + fd166;
    fd172 = fd146 + fd158;
    fd173 = MUL_C(COEF_CONST(1.1758756024193588), fd146);
    fd174 = MUL_F(FRAC_CONST(-0.9807852804032304), fd172);
    fd175 = MUL_F(FRAC_CONST(-0.7856949583871021), fd158);
    fd176 = fd173 + fd174;
    fd177 = fd175 - fd174;
    fd178 = fd150 + fd154;
    fd179 = MUL_C(COEF_CONST(1.3870398453221473), fd150);
    fd180 = MUL_F(FRAC_CONST(-0.8314696123025455), fd178);
    fd181 = MUL_F(FRAC_CONST(-0.2758993792829436), fd154);
    fd182 = fd179 + fd180;
    fd183 = fd181 - fd180;
    fd184 = fd177 - fd183;
    fd185 = fd177 + fd183;
    fd186 = MUL_F(FRAC_CONST(0.7071067811865476), fd184);
    fd187 = fd176 - fd182;
    fd188 = fd176 + fd182;
    fd189 = MUL_F(FRAC_CONST(0.7071067811865476), fd188);
    fd190 = fd186 - fd189;
    fd191 = fd186 + fd189;
    fd192 = fd169 - fd185;
    fd193 = fd169 + fd185;
    fd194 = fd171 - fd191;
    fd195 = fd171 + fd191;
    fd196 = fd170 - fd190;
    fd197 = fd170 + fd190;
    fd198 = fd168 - fd187;
    fd199 = fd168 + fd187;
    fd200 = MUL_F(FRAC_CONST(0.7071067811865476), fd151);
    fd201 = fd30 - fd200;
    fd202 = fd30 + fd200;
    fd203 = fd155 + fd147;
    fd204 = MUL_C(COEF_CONST(1.3065629648763766), fd155);
    fd205 = MUL_F(FRAC_CONST(-0.9238795325112866), fd203);
    fd206 = MUL_F(FRAC_CONST(-0.5411961001461967), fd147);
    fd207 = fd204 + fd205;
    fd208 = fd206 - fd205;
    fd209 = fd202 - fd208;
    fd210 = fd202 + fd208;
    fd211 = fd201 - fd207;
    fd212 = fd201 + fd207;
    fd213 = fd157 + fd145;
    fd214 = MUL_C(COEF_CONST(1.1758756024193588), fd157);
    fd215 = MUL_F(FRAC_CONST(-0.9807852804032304), fd213);
    fd216 = MUL_F(FRAC_CONST(-0.7856949583871021), fd145);
    fd217 = fd214 + fd215;
    fd218 = fd216 - fd215;
    fd219 = fd153 + fd149;
    fd220 = MUL_C(COEF_CONST(1.3870398453221473), fd153);
    fd221 = MUL_F(FRAC_CONST(-0.8314696123025455), fd219);
    fd222 = MUL_F(FRAC_CONST(-0.2758993792829436), fd149);
    fd223 = fd220 + fd221;
    fd224 = fd222 - fd221;
    fd225 = fd218 - fd224;
    fd226 = fd218 + fd224;
    fd227 = MUL_F(FRAC_CONST(0.7071067811865476), fd225);
    fd228 = fd217 - fd223;
    fd229 = fd217 + fd223;
    fd230 = MUL_F(FRAC_CONST(0.7071067811865476), fd229);
    fd231 = fd227 - fd230;
    fd232 = fd227 + fd230;
    fd233 = fd210 - fd226;
    fd234 = fd210 + fd226;
    fd235 = fd212 - fd232;
    fd236 = fd212 + fd232;
    fd237 = fd211 - fd231;
    fd238 = fd211 + fd231;
    fd239 = fd209 - fd228;
    fd240 = fd209 + fd228;
    fd241 = fd234 + fd193;
    fd242 = MUL_F(FRAC_CONST(-0.9497277818777543), fd234);
    fd243 = MUL_F(FRAC_CONST(0.9987954562051724), fd241);
    fd244 = MUL_C(COEF_CONST(1.0478631305325905), fd193);
    y[1] = fd242 + fd243;
    y[31] = fd244 - fd243;
    fd247 = fd195 - fd236;
    fd248 = MUL_F(FRAC_CONST(-0.8424460355094192), fd236);
    fd249 = MUL_F(FRAC_CONST(0.9891765099647810), fd247);
    fd250 = MUL_C(COEF_CONST(1.1359069844201428), fd195);
    y[3] = fd249 - fd248;
    y[29] = fd250 - fd249;
    fd253 = fd238 + fd197;
    fd254 = MUL_F(FRAC_CONST(-0.7270510732912801), fd238);
    fd255 = MUL_F(FRAC_CONST(0.9700312531945440), fd253);
    fd256 = MUL_C(COEF_CONST(1.2130114330978079), fd197);
    y[5] = fd254 + fd255;
    y[27] = fd256 - fd255;
    fd259 = fd199 - fd240;
    fd260 = MUL_F(FRAC_CONST(-0.6046542117908007), fd240);
    fd261 = MUL_F(FRAC_CONST(0.9415440651830208), fd259);
    fd262 = MUL_C(COEF_CONST(1.2784339185752409), fd199);
    y[7] = fd261 - fd260;
    y[25] = fd262 - fd261;
    fd265 = fd239 + fd198;
    fd266 = MUL_F(FRAC_CONST(-0.4764341996931611), fd239);
    fd267 = MUL_F(FRAC_CONST(0.9039892931234433), fd265);
    fd268 = MUL_C(COEF_CONST(1.3315443865537255), fd198);
    y[9] = fd266 + fd267;
    y[23] = fd268 - fd267;
    fd271 = fd196 - fd237;
    fd272 = MUL_F(FRAC_CONST(-0.3436258658070505), fd237);
    fd273 = MUL_F(FRAC_CONST(0.8577286100002721), fd271);
    fd274 = MUL_C(COEF_CONST(1.3718313541934939), fd196);
    y[11] = fd273 - fd272;
    y[21] = fd274 - fd273;
    fd277 = fd235 + fd194;
    fd278 = MUL_F(FRAC_CONST(-0.2075082269882114), fd235);
    fd279 = MUL_F(FRAC_CONST(0.8032075314806448), fd277);
    fd280 = MUL_C(COEF_CONST(1.3989068359730783), fd194);
    y[13] = fd278 + fd279;
    y[19] = fd280 - fd279;
    fd283 = fd192 - fd233;
    fd284 = MUL_F(FRAC_CONST(-0.0693921705079408), fd233);
    fd285 = MUL_F(FRAC_CONST(0.7409511253549591), fd283);
    fd286 = MUL_C(COEF_CONST(1.4125100802019774), fd192);
    y[15] = fd285 - fd284;
    y[17] = fd286 - fd285;
}

#else


#define n 32
#define log2n 5

// w_array_real[i] = cos(2*M_PI*i/32)
static const real_t w_array_real[] = {
    FRAC_CONST(1.000000000000000), FRAC_CONST(0.980785279337272),
    FRAC_CONST(0.923879528329380), FRAC_CONST(0.831469603195765),
    FRAC_CONST(0.707106765732237), FRAC_CONST(0.555570210304169),
    FRAC_CONST(0.382683402077046), FRAC_CONST(0.195090284503576),
    FRAC_CONST(0.000000000000000), FRAC_CONST(-0.195090370246552),
    FRAC_CONST(-0.382683482845162), FRAC_CONST(-0.555570282993553),
    FRAC_CONST(-0.707106827549476), FRAC_CONST(-0.831469651765257),
    FRAC_CONST(-0.923879561784627), FRAC_CONST(-0.980785296392607)
};

// w_array_imag[i] = sin(-2*M_PI*i/32)
static const real_t w_array_imag[] = {
    FRAC_CONST(0.000000000000000), FRAC_CONST(-0.195090327375064),
    FRAC_CONST(-0.382683442461104), FRAC_CONST(-0.555570246648862),
    FRAC_CONST(-0.707106796640858), FRAC_CONST(-0.831469627480512),
    FRAC_CONST(-0.923879545057005), FRAC_CONST(-0.980785287864940),
    FRAC_CONST(-1.000000000000000), FRAC_CONST(-0.980785270809601),
    FRAC_CONST(-0.923879511601754), FRAC_CONST(-0.831469578911016),
    FRAC_CONST(-0.707106734823616), FRAC_CONST(-0.555570173959476),
    FRAC_CONST(-0.382683361692986), FRAC_CONST(-0.195090241632088)
};

// FFT decimation in frequency
// 4*16*2+16=128+16=144 multiplications
// 6*16*2+10*8+4*16*2=192+80+128=400 additions
static void fft_dif(real_t * Real, real_t * Imag)
{
    real_t w_real, w_imag; // For faster access
    real_t point1_real, point1_imag, point2_real, point2_imag; // For faster access
    unsigned int j, i, i2, w_index; // Counters

    // First 2 stages of 32 point FFT decimation in frequency
    // 4*16*2=64*2=128 multiplications
    // 6*16*2=96*2=192 additions
	// Stage 1 of 32 point FFT decimation in frequency
    for (i = 0; i < 16; i++)
    {
        point1_real = Real[i];
        point1_imag = Imag[i];
        i2 = i+16;
        point2_real = Real[i2];
        point2_imag = Imag[i2];

        w_real = w_array_real[i];
        w_imag = w_array_imag[i];

        // temp1 = x[i] - x[i2]
        point1_real -= point2_real;
        point1_imag -= point2_imag;

        // x[i1] = x[i] + x[i2]
        Real[i] += point2_real;
        Imag[i] += point2_imag;

        // x[i2] = (x[i] - x[i2]) * w
        Real[i2] = (MUL_F(point1_real,w_real) - MUL_F(point1_imag,w_imag));
        Imag[i2] = (MUL_F(point1_real,w_imag) + MUL_F(point1_imag,w_real));
     }
    // Stage 2 of 32 point FFT decimation in frequency
    for (j = 0, w_index = 0; j < 8; j++, w_index += 2)
    {
        w_real = w_array_real[w_index];
        w_imag = w_array_imag[w_index];

    	i = j;
        point1_real = Real[i];
        point1_imag = Imag[i];
        i2 = i+8;
        point2_real = Real[i2];
        point2_imag = Imag[i2];

        // temp1 = x[i] - x[i2]
        point1_real -= point2_real;
        point1_imag -= point2_imag;

        // x[i1] = x[i] + x[i2]
        Real[i] += point2_real;
        Imag[i] += point2_imag;

        // x[i2] = (x[i] - x[i2]) * w
        Real[i2] = (MUL_F(point1_real,w_real) - MUL_F(point1_imag,w_imag));
        Imag[i2] = (MUL_F(point1_real,w_imag) + MUL_F(point1_imag,w_real));

        i = j+16;
        point1_real = Real[i];
        point1_imag = Imag[i];
        i2 = i+8;
        point2_real = Real[i2];
        point2_imag = Imag[i2];

        // temp1 = x[i] - x[i2]
        point1_real -= point2_real;
        point1_imag -= point2_imag;

        // x[i1] = x[i] + x[i2]
        Real[i] += point2_real;
        Imag[i] += point2_imag;

        // x[i2] = (x[i] - x[i2]) * w
        Real[i2] = (MUL_F(point1_real,w_real) - MUL_F(point1_imag,w_imag));
        Imag[i2] = (MUL_F(point1_real,w_imag) + MUL_F(point1_imag,w_real));
    }

    // Stage 3 of 32 point FFT decimation in frequency
    // 2*4*2=16 multiplications
    // 4*4*2+6*4*2=10*8=80 additions
    for (i = 0; i < n; i += 8)
    {
        i2 = i+4;
        point1_real = Real[i];
        point1_imag = Imag[i];

        point2_real = Real[i2];
        point2_imag = Imag[i2];

        // out[i1] = point1 + point2
        Real[i] += point2_real;
        Imag[i] += point2_imag;

        // out[i2] = point1 - point2
        Real[i2] = point1_real - point2_real;
        Imag[i2] = point1_imag - point2_imag;
    }
    w_real = w_array_real[4]; // = sqrt(2)/2
    // w_imag = -w_real; // = w_array_imag[4]; // = -sqrt(2)/2
    for (i = 1; i < n; i += 8)
    {
        i2 = i+4;
        point1_real = Real[i];
        point1_imag = Imag[i];

        point2_real = Real[i2];
        point2_imag = Imag[i2];

        // temp1 = x[i] - x[i2]
        point1_real -= point2_real;
        point1_imag -= point2_imag;

        // x[i1] = x[i] + x[i2]
        Real[i] += point2_real;
        Imag[i] += point2_imag;

        // x[i2] = (x[i] - x[i2]) * w
        Real[i2] = MUL_F(point1_real+point1_imag, w_real);
        Imag[i2] = MUL_F(point1_imag-point1_real, w_real);
    }
    for (i = 2; i < n; i += 8)
    {
        i2 = i+4;
        point1_real = Real[i];
        point1_imag = Imag[i];

        point2_real = Real[i2];
        point2_imag = Imag[i2];

        // x[i] = x[i] + x[i2]
        Real[i] += point2_real;
        Imag[i] += point2_imag;

        // x[i2] = (x[i] - x[i2]) * (-i)
        Real[i2] = point1_imag - point2_imag;
        Imag[i2] = point2_real - point1_real;
    }
    w_real = w_array_real[12]; // = -sqrt(2)/2
    // w_imag = w_real; // = w_array_imag[12]; // = -sqrt(2)/2
    for (i = 3; i < n; i += 8)
    {
        i2 = i+4;
        point1_real = Real[i];
        point1_imag = Imag[i];

        point2_real = Real[i2];
        point2_imag = Imag[i2];

        // temp1 = x[i] - x[i2]
        point1_real -= point2_real;
        point1_imag -= point2_imag;

        // x[i1] = x[i] + x[i2]
        Real[i] += point2_real;
        Imag[i] += point2_imag;

        // x[i2] = (x[i] - x[i2]) * w
        Real[i2] = MUL_F(point1_real-point1_imag, w_real);
        Imag[i2] = MUL_F(point1_real+point1_imag, w_real);
    }


    // Stage 4 of 32 point FFT decimation in frequency (no multiplications)
    // 16*4=64 additions
    for (i = 0; i < n; i += 4)
    {
        i2 = i+2;
        point1_real = Real[i];
        point1_imag = Imag[i];

        point2_real = Real[i2];
        point2_imag = Imag[i2];

        // x[i1] = x[i] + x[i2]
        Real[i] += point2_real;
        Imag[i] += point2_imag;

        // x[i2] = x[i] - x[i2]
        Real[i2] = point1_real - point2_real;
        Imag[i2] = point1_imag - point2_imag;
    }
    for (i = 1; i < n; i += 4)
    {
        i2 = i+2;
        point1_real = Real[i];
        point1_imag = Imag[i];

        point2_real = Real[i2];
        point2_imag = Imag[i2];

        // x[i] = x[i] + x[i2]
        Real[i] += point2_real;
        Imag[i] += point2_imag;

        // x[i2] = (x[i] - x[i2]) * (-i)
        Real[i2] = point1_imag - point2_imag;
        Imag[i2] = point2_real - point1_real;
    }

    // Stage 5 of 32 point FFT decimation in frequency (no multiplications)
    // 16*4=64 additions
    for (i = 0; i < n; i += 2)
    {
        i2 = i+1;
        point1_real = Real[i];
        point1_imag = Imag[i];

        point2_real = Real[i2];
        point2_imag = Imag[i2];

        // out[i1] = point1 + point2
        Real[i] += point2_real;
        Imag[i] += point2_imag;

        // out[i2] = point1 - point2
        Real[i2] = point1_real - point2_real;
        Imag[i2] = point1_imag - point2_imag;
    }

#ifdef REORDER_IN_FFT
    FFTReorder(Real, Imag);
#endif // #ifdef REORDER_IN_FFT
}
#undef n
#undef log2n

static const real_t dct4_64_tab[] = {
    COEF_CONST(0.999924719333649), COEF_CONST(0.998118102550507),
    COEF_CONST(0.993906974792480), COEF_CONST(0.987301409244537),
    COEF_CONST(0.978317379951477), COEF_CONST(0.966976463794708),
    COEF_CONST(0.953306019306183), COEF_CONST(0.937339007854462),
    COEF_CONST(0.919113874435425), COEF_CONST(0.898674488067627),
    COEF_CONST(0.876070082187653), COEF_CONST(0.851355195045471),
    COEF_CONST(0.824589252471924), COEF_CONST(0.795836925506592),
    COEF_CONST(0.765167236328125), COEF_CONST(0.732654273509979),
    COEF_CONST(0.698376238346100), COEF_CONST(0.662415742874146),
    COEF_CONST(0.624859452247620), COEF_CONST(0.585797846317291),
    COEF_CONST(0.545324981212616), COEF_CONST(0.503538429737091),
    COEF_CONST(0.460538715124130), COEF_CONST(0.416429549455643),
    COEF_CONST(0.371317148208618), COEF_CONST(0.325310230255127),
    COEF_CONST(0.278519600629807), COEF_CONST(0.231058135628700),
    COEF_CONST(0.183039888739586), COEF_CONST(0.134580686688423),
    COEF_CONST(0.085797272622585), COEF_CONST(0.036807164549828),
    COEF_CONST(-1.012196302413940), COEF_CONST(-1.059438824653626),
    COEF_CONST(-1.104129195213318), COEF_CONST(-1.146159529685974),
    COEF_CONST(-1.185428738594055), COEF_CONST(-1.221842169761658),
    COEF_CONST(-1.255311965942383), COEF_CONST(-1.285757660865784),
    COEF_CONST(-1.313105940818787), COEF_CONST(-1.337290763854981),
    COEF_CONST(-1.358253836631775), COEF_CONST(-1.375944852828980),
    COEF_CONST(-1.390321016311646), COEF_CONST(-1.401347875595093),
    COEF_CONST(-1.408998727798462), COEF_CONST(-1.413255214691162),
    COEF_CONST(-1.414107084274292), COEF_CONST(-1.411552190780640),
    COEF_CONST(-1.405596733093262), COEF_CONST(-1.396255016326904),
    COEF_CONST(-1.383549690246582), COEF_CONST(-1.367511272430420),
    COEF_CONST(-1.348178386688232), COEF_CONST(-1.325597524642944),
    COEF_CONST(-1.299823284149170), COEF_CONST(-1.270917654037476),
    COEF_CONST(-1.238950133323669), COEF_CONST(-1.203998088836670),
    COEF_CONST(-1.166145324707031), COEF_CONST(-1.125483393669128),
    COEF_CONST(-1.082109928131104), COEF_CONST(-1.036129593849182),
    COEF_CONST(-0.987653195858002), COEF_CONST(-0.936797380447388),
    COEF_CONST(-0.883684754371643), COEF_CONST(-0.828443288803101),
    COEF_CONST(-0.771206021308899), COEF_CONST(-0.712110757827759),
    COEF_CONST(-0.651300072669983), COEF_CONST(-0.588920354843140),
    COEF_CONST(-0.525121808052063), COEF_CONST(-0.460058242082596),
    COEF_CONST(-0.393886327743530), COEF_CONST(-0.326765477657318),
    COEF_CONST(-0.258857429027557), COEF_CONST(-0.190325915813446),
    COEF_CONST(-0.121335685253143), COEF_CONST(-0.052053272724152),
    COEF_CONST(0.017354607582092), COEF_CONST(0.086720645427704),
    COEF_CONST(0.155877828598022), COEF_CONST(0.224659323692322),
    COEF_CONST(0.292899727821350), COEF_CONST(0.360434412956238),
    COEF_CONST(0.427100926637650), COEF_CONST(0.492738455533981),
    COEF_CONST(0.557188928127289), COEF_CONST(0.620297133922577),
    COEF_CONST(0.681910991668701), COEF_CONST(0.741881847381592),
    COEF_CONST(0.800065577030182), COEF_CONST(0.856321990489960),
    COEF_CONST(0.910515367984772), COEF_CONST(0.962515234947205),
    COEF_CONST(1.000000000000000), COEF_CONST(0.998795449733734),
    COEF_CONST(0.995184719562531), COEF_CONST(0.989176511764526),
    COEF_CONST(0.980785250663757), COEF_CONST(0.970031261444092),
    COEF_CONST(0.956940352916718), COEF_CONST(0.941544055938721),
    COEF_CONST(0.923879504203796), COEF_CONST(0.903989315032959),
    COEF_CONST(0.881921231746674), COEF_CONST(0.857728600502014),
    COEF_CONST(0.831469595432281), COEF_CONST(0.803207516670227),
    COEF_CONST(0.773010432720184), COEF_CONST(0.740951120853424),
    COEF_CONST(0.707106769084930), COEF_CONST(0.671558916568756),
    COEF_CONST(0.634393274784088), COEF_CONST(0.595699310302734),
    COEF_CONST(0.555570185184479), COEF_CONST(0.514102697372437),
    COEF_CONST(0.471396654844284), COEF_CONST(0.427555114030838),
    COEF_CONST(0.382683426141739), COEF_CONST(0.336889833211899),
    COEF_CONST(0.290284633636475), COEF_CONST(0.242980122566223),
    COEF_CONST(0.195090234279633), COEF_CONST(0.146730497479439),
    COEF_CONST(0.098017133772373), COEF_CONST(0.049067649990320),
    COEF_CONST(-1.000000000000000), COEF_CONST(-1.047863125801086),
    COEF_CONST(-1.093201875686646), COEF_CONST(-1.135906934738159),
    COEF_CONST(-1.175875544548035), COEF_CONST(-1.213011503219605),
    COEF_CONST(-1.247225046157837), COEF_CONST(-1.278433918952942),
    COEF_CONST(-1.306562900543213), COEF_CONST(-1.331544399261475),
    COEF_CONST(-1.353317975997925), COEF_CONST(-1.371831417083740),
    COEF_CONST(-1.387039899826050), COEF_CONST(-1.398906826972961),
    COEF_CONST(-1.407403707504273), COEF_CONST(-1.412510156631470),
    COEF_CONST(0), COEF_CONST(-1.412510156631470),
    COEF_CONST(-1.407403707504273), COEF_CONST(-1.398906826972961),
    COEF_CONST(-1.387039899826050), COEF_CONST(-1.371831417083740),
    COEF_CONST(-1.353317975997925), COEF_CONST(-1.331544399261475),
    COEF_CONST(-1.306562900543213), COEF_CONST(-1.278433918952942),
    COEF_CONST(-1.247225046157837), COEF_CONST(-1.213011384010315),
    COEF_CONST(-1.175875544548035), COEF_CONST(-1.135907053947449),
    COEF_CONST(-1.093201875686646), COEF_CONST(-1.047863125801086),
    COEF_CONST(-1.000000000000000), COEF_CONST(-0.949727773666382),
    COEF_CONST(-0.897167563438416), COEF_CONST(-0.842446029186249),
    COEF_CONST(-0.785694956779480), COEF_CONST(-0.727051079273224),
    COEF_CONST(-0.666655659675598), COEF_CONST(-0.604654192924500),
    COEF_CONST(-0.541196048259735), COEF_CONST(-0.476434230804443),
    COEF_CONST(-0.410524487495422), COEF_CONST(-0.343625843524933),
    COEF_CONST(-0.275899350643158), COEF_CONST(-0.207508206367493),
    COEF_CONST(-0.138617098331451), COEF_CONST(-0.069392144680023),
    COEF_CONST(0), COEF_CONST(0.069392263889313),
    COEF_CONST(0.138617157936096), COEF_CONST(0.207508206367493),
    COEF_CONST(0.275899469852448), COEF_CONST(0.343625962734222),
    COEF_CONST(0.410524636507034), COEF_CONST(0.476434201002121),
    COEF_CONST(0.541196107864380), COEF_CONST(0.604654192924500),
    COEF_CONST(0.666655719280243), COEF_CONST(0.727051138877869),
    COEF_CONST(0.785695075988770), COEF_CONST(0.842446029186249),
    COEF_CONST(0.897167563438416), COEF_CONST(0.949727773666382)
};

/* size 64 only! */
void dct4_kernel(real_t * in_real, real_t * in_imag, real_t * out_real, real_t * out_imag)
{
    // Tables with bit reverse values for 5 bits, bit reverse of i at i-th position
    const unsigned char bit_rev_tab[32] = { 0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31 };
    unsigned int i, i_rev;

    /* Step 2: modulate */
    // 3*32=96 multiplications
    // 3*32=96 additions
    for (i = 0; i < 32; i++)
    {
    	real_t x_re, x_im, tmp;
    	x_re = in_real[i];
    	x_im = in_imag[i];
        tmp =        MUL_C(x_re + x_im, dct4_64_tab[i]);
        in_real[i] = MUL_C(x_im, dct4_64_tab[i + 64]) + tmp;
        in_imag[i] = MUL_C(x_re, dct4_64_tab[i + 32]) + tmp;
    }

    /* Step 3: FFT, but with output in bit reverse order */
    fft_dif(in_real, in_imag);

    /* Step 4: modulate + bitreverse reordering */
    // 3*31+2=95 multiplications
    // 3*31+2=95 additions
    for (i = 0; i < 16; i++)
    {
    	real_t x_re, x_im, tmp;
    	i_rev = bit_rev_tab[i];
    	x_re = in_real[i_rev];
    	x_im = in_imag[i_rev];

        tmp =         MUL_C(x_re + x_im, dct4_64_tab[i + 3*32]);
        out_real[i] = MUL_C(x_im, dct4_64_tab[i + 5*32]) + tmp;
        out_imag[i] = MUL_C(x_re, dct4_64_tab[i + 4*32]) + tmp;
    }
    // i = 16, i_rev = 1 = rev(16);
    out_imag[16] = MUL_C(in_imag[1] - in_real[1], dct4_64_tab[16 + 3*32]);
    out_real[16] = MUL_C(in_real[1] + in_imag[1], dct4_64_tab[16 + 3*32]);
    for (i = 17; i < 32; i++)
    {
    	real_t x_re, x_im, tmp;
    	i_rev = bit_rev_tab[i];
    	x_re = in_real[i_rev];
    	x_im = in_imag[i_rev];
        tmp =         MUL_C(x_re + x_im, dct4_64_tab[i + 3*32]);
        out_real[i] = MUL_C(x_im, dct4_64_tab[i + 5*32]) + tmp;
        out_imag[i] = MUL_C(x_re, dct4_64_tab[i + 4*32]) + tmp;
    }

}

#endif

#endif
