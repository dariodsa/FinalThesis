#include "../src/structures/index.h"
#define SELECT 257
#define UNION 258
#define DISTINCT 259
#define ALL 260
#define FROM 261
#define WHERE 262
#define LIMIT 263
#define QUOTED_STRING 264
#define OFFSET 265
#define ONLY 266
#define HAVING 267
#define BY 268
#define GROUP 269
#define ORDER 270
#define JOIN 271
#define NATURAL 272
#define LEFT 273
#define RIGHT 274
#define INNER 275
#define FULL 276
#define OUTER 277
#define USING 278
#define CMP 279
#define BETWEEN 280
#define NULL_STR 281
#define IN 282
#define EXISTS 283
#define CASE 284
#define THEN 285
#define ELSE 286
#define VALUES 287
#define INSERT 288
#define INTO 289
#define CREATE 290
#define TABLE 291
#define UNIQUE 292
#define PRIMARY 293
#define FOREIGN 294
#define KEY 295
#define CONSTRAINT 296
#define INDEX 297
#define ASC 298
#define DESC 299
#define NAME 300
#define NUMBER 301
#define ENUMBER 302
#define AS 303
#define CROSS 304
#define DATA_TYPE 305
#define ALTER 306
#define ADD 307
#define END 308
#define WHEN 309
#define ANY 310
#define SOME 311
#define AGG_FUNCTION 312
#define CHECK 313
#define UPDATE 314
#define DELETE 315
#define SET 316
#define DEFAULT 317
#define ON 318
#define CASCADE 319
#define REFERENCES 320
#define IS 321
#define LIKE 322
#define SINGLE_QUOTED_STRING 323
#define OR 324
#define AND 325
#define NOT 326
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union
{
   double fvalue;
   char *text;  
   Column* column;
   Index* index;
   Table* table;

   int number;

    vector<char*>* names;



} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;
