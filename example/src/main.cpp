#include <tonc.h>
#include <string>
#include <vector>

extern "C" {
// (0) Include the headers
#include "../lib/fatfs/ff.h"
#include "../lib/flashcartio.h"
}

#define MAX_ENTRIES 20

void listDir(std::vector<FILINFO> items,
             u32 selected,
             u32 offset = 0,
             u32 count = MAX_ENTRIES);
std::vector<FILINFO> readDir(std::string path);
void log(std::string text);
void halt(std::string text);
u16 waitFor(u16 key);
const std::string names[] = {"-", "Everdrive", "EZ Flash"};

void init() {
  REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;
  tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
  irq_init(NULL);
  irq_add(II_VBLANK, NULL);
  REG_KEYCNT = 0b1100000000001111;
  irq_add(II_KEYPAD, SoftReset);

  // (1) Initialize the library
  if (!flashcartio_activate())
    halt("No flashcart detected!");
}

int main() {
  init();
  log("Detected flashcart: " + names[active_flashcart] +
      "\n\nPress A and open a text file!\nPress B to go back!");
  waitFor(KEY_A);

  // (2) Use FatFs functions
  FATFS fatfs;
  FRESULT fr = f_mount(&fatfs, "", 1);
  if (fr > 0)
    halt("mount failed!");

  std::string path = "";
  u32 selected = 0;
  std::vector<FILINFO> files = readDir(path);
  listDir(files, selected);

  while (true) {
    u16 keys = waitFor(KEY_DOWN | KEY_UP | KEY_A | KEY_B);
    if (keys == KEY_DOWN) {
      selected = (selected + 1) % files.size();
    } else if (keys == KEY_UP) {
      selected = selected == 0 ? files.size() - 1 : selected - 1;
    } else if (keys == KEY_A) {
      auto selectedItem = files[selected];
      if (selectedItem.fattrib & AM_DIR) {
        path += "/" + std::string(selectedItem.fname);
        files = readDir(path);
        selected = 0;
      } else {
        if (selectedItem.fsize < 1024 * 10) {
          auto filePath = path + "/" + selectedItem.fname;
          FIL fil;
          f_open(&fil, filePath.c_str(), FA_READ);
          if (fr > 0)
            halt("open failed!");

          std::string content = "";
          char line[128];
          while (f_gets(line, sizeof(line), &fil))
            content += std::string(line) + "\n";
          log(content);
        } else {
          log("Nah, too big! Use a text file!");
        }

        waitFor(KEY_B);
      }
    } else if (keys == KEY_B) {
      if (path != "/") {
        path = "/";
        files = readDir(path);
        selected = 0;
      }
    }

    VBlankIntrWait();
    listDir(files, selected, max(selected - MAX_ENTRIES / 2, 0));
  }

  return 0;
}

void listDir(std::vector<FILINFO> items, u32 selected, u32 offset, u32 count) {
  std::string output = "";
  for (u32 i = offset; i < items.size(); i++) {
    auto item = items[i];
    output += std::string(selected == i ? ">>" : "") +
              (item.fattrib & AM_DIR ? "[" : "") + std::string(item.fname) +
              (item.fattrib & AM_DIR ? "]" : "") + "\n";
    if (i - offset + 1 >= count)
      break;
  }
  log(output);
}

std::vector<FILINFO> readDir(std::string path) {
  auto items = std::vector<FILINFO>{};

  DIR dir;
  FRESULT fr = f_opendir(&dir, path.c_str());
  if (fr > 0)
    halt("opendir failed!");
  FILINFO fno;
  while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0] != 0)
    items.push_back(fno);
  fr = f_closedir(&dir);
  if (fr > 0)
    halt("closedir failed!");

  return items;
}

void log(std::string text) {
  tte_erase_screen();
  tte_write("#{P:0,0}");
  tte_write(text.c_str());
}

void halt(std::string text) {
  log(text);
  while (true)
    ;
}

u16 waitFor(u16 key) {
  u16 keys;

  do {
    keys = ~REG_KEYS & KEY_ANY;
  } while ((keys & key));

  do {
    keys = ~REG_KEYS & KEY_ANY;
  } while (!(keys & key));

  return keys;
}