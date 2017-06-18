/*
 *            main.c
 * written by Shinichi Awamoto, 2017
 * 
 * 各種ハードウェア初期化関数の呼び出し
 */

#include "common.h"
#include "multiboot2.h"
#include "acpi.h"
#include "framebuffer.h"

uint32_t multiboot_info;

void cmain() {
  // TODO check multiboot2 magic number

  // multiboot_info について
  // multiboot_info にはentryの直後でebxレジスタが代入されている。
  // この値は本来32bit物理アドレスだが、現在は1GBをストレートマップ
  // しているため、1GB以下の空間については物理＝リニア＝論理とみなせる。
  // かつ、64bitのうちの上位32bitについては0で埋めたとみなせば、
  // 期待通りアクセスできる。
  // よって、uint32_tをポインタ(64bit)にキャストしても正常に動作する。

  // コードはmultiboot specification 2 仕様書より
  for (struct multiboot_tag *tag = (struct multiboot_tag *)(uint64_t)(multiboot_info + 8); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 0b111) & ~0b111))) {
    switch(tag->type) {
    case MULTIBOOT_TAG_TYPE_ACPI_OLD:
      {
        // ACPI RSDPテーブルの取得
        struct multiboot_tag_old_acpi *acpi = (struct multiboot_tag_old_acpi *)tag;
        acpi_init((struct rsdp_descriptor *)acpi->rsdp);
      }
      break;
    case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
      {
        // framebuffer tagの取得
        framebuffer_init((struct multiboot_tag_framebuffer_common *)tag);
      }
      break;
    default:
      break;
    }
  }
  
  // TODO ここにコードを追加

  while(1) {
    __asm__ volatile("hlt;");
  }
}

void panic() {
  while(1) {
    __asm__ volatile("cli;hlt;nop;");
  }
}
