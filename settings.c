#include <stdbool.h>

// exists if pattern is found
bool exit_if_found = true;

// use "minecraft:bedrock_floor" or "minecraft:bedrock_roof"
char *random_type = "minecraft:bedrock_floor";

// the world seed you can get it with /seed or a seedcracker like seedcrackerX
long world_seed = -6965625275890949177L;

// the y level you are targeting 
// for overworld use -60
// for nether roof use 123
// for nether floor use 4
int y = -60;

// the search pattern 
// # means bedrock here 
// . means no bedrock 
// \n means next row 
// ? means i dunno 
 const char *search_for =
"........\n"
"##.#....\n"
"...#..##\n"
".#..#.#.\n"
".##.....\n"
"#...#.#.\n"
"#.....#.\n"
"..#.#...\n"
"#..#....\n";


// examples
// these use the seed -6965625275890949177
// const char *search_for =
//"#.##...#.\n"
//"....##.#.\n"
//".#..#....\n"
//"#.....##.\n"
//".#.#.#...\n"
//".........\n"
//"..##.##..\n"
//"......#.."; // x 8 z -10

// const char *search_for =
//"....#..#\n"
//"...#.#..\n"
//".#.....#\n"
//".#.#...#\n"
//".....##.\n"
//".#.#..#.\n"
//"##..#...\n"
//"....#.##\n"
//"........"; // x 1 z -10

// const char *search_for =
//"..#......\n"
//"..##.##..\n"
//".........\n"
//"...#.#.#.\n"
//".##.....#\n"
//"....#..#.\n"
//".#.##....\n"
//".#...##.#"; // x 1 z -2

// const char *search_for =
//"........\n"
//"##.#....\n"
//"...#..##\n"
//".#..#.#.\n"
//".##.....\n"
//"#...#.#.\n"
//"#.....#.\n"
//"..#.#...\n"
//"#..#....\n"; // x 8 z -2

