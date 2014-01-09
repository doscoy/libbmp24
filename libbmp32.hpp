
#ifndef LIB_BMP32_HPP
#define LIB_BMP32_HPP

#define LIBBMP_VERSION  1000


#include <cstdlib>
#include <iostream>
#include <cstdint>
#include <fstream>


namespace libbmp32 {


//------------------------------------------------------------------------------------------
//  ビットマップファイルヘッダ
#pragma pack(1)

struct BitmapFileHeader {
    uint16_t type_;
    uint32_t size_;
    uint16_t reserved1_;
    uint16_t reserved2_;
    uint32_t offbits_;
};

struct BitmapInfoHeader{
  uint32_t size_;
  int32_t width_;
  int32_t heigth_;
  uint16_t planes_;
  uint16_t bit_count_;
  uint32_t compression_;
  uint32_t size_image_;
  int32_t pels_per_meter_x_;
  int32_t pels_per_meter_y_;
  uint32_t clr_used_;
  uint32_t clr_important_;
};


#pragma pack()


struct Bitmap
{
    Bitmap()
        : file_header_()
        , info_header_()
        , data_(nullptr)
    {}

    ~Bitmap() {
        delete data_;
    }


    int32_t getWidth() const {
        return info_header_.width_;
    }

    int32_t getHeight() const {
        return info_header_.heigth_;
    }

    uint32_t getImageSize() const {
        return info_header_.size_image_;
    }

    BitmapFileHeader file_header_;
    BitmapInfoHeader info_header_;
    uint8_t* data_;
};



/**
 *  ビットマップファイルヘッダ構築.
 */
void setupBitmapFileHeader(
    BitmapFileHeader* const file_header,
    const int img_size
){
    file_header->type_ = *(uint16_t*)"BM";  // TODO 定数アクセスする
    file_header->size_ = img_size + sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    file_header->reserved1_ = 0;
    file_header->reserved2_ = 0;
    file_header->offbits_ = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
}

/**
 *  ビットマップインフォヘッダ構築.
 */
void setupBitmapInfoHeader(
    BitmapInfoHeader* const info_header,
    const int x,
    const int y,
    const int img_size
){
    info_header->size_ = sizeof(BitmapInfoHeader);
    info_header->width_ = x;
    info_header->heigth_ = y;
    info_header->planes_ = 1;
    info_header->bit_count_ = 24;
    info_header->compression_ = 0;
    info_header->size_image_ = img_size;
    info_header->pels_per_meter_x_ = 0;
    info_header->pels_per_meter_y_ = 0;
    info_header->clr_used_ = 0;
    info_header->clr_important_ = 0;
}



//------------------------------------------------------------------------------------------
//  ビットマップ書き出し. fstream ver.
bool serializeBmp(
    std::ofstream* file, 
    const Accessor* const accessor
){
    if(file == nullptr || accessor == nullptr) {
        return false;
    }

    //  ----------------------------------
    //  ヘッダ作成
    int x = accessor->getWidth();
    int y = accessor->getHeight();
    int padding = (x * 3 + 3) / 4 * 4 - x * 3;      // 32bit境界条件によるパディング
    int img_size = (x * 3 + padding) * y;           // 出力される場増データサイズ

    //  ファイルヘッダ
    BitmapFileHeader file_header;
    setupBitmapFileHeader(&file_header, img_size);

    //  インフォヘッダ
    BitmapInfoHeader info_header;
    setupBitmapInfoHeader(
        &info_header,
        x,
        y,
        img_size
    );

    //  ----------------------------------
    //  ヘッダ出力
    file->write((char*)&file_header, sizeof(BitmapFileHeader));
    file->write((char*)&info_header, sizeof(BitmapInfoHeader));

    //  内容出力
    file->write((char*)accessor->data_, img_size);

    //  成功
    return true;

}


//------------------------------------------------------------------------------------------
/**
 *  ビットマップの整合性チェック.
 */
bool isVaildBmp(
    const BitmapFileHeader* const file_header,
    const BitmapInfoHeader* const info_header
){
    if (file_header->type_ != *(uint16_t*)"BM") { // TODO 定数判定に変える
        return false;
    }
    if (info_header->size_ != sizeof(BitmapInfoHeader)) {
        return false;
    }
    if (info_header->compression_ != 0) {
        return false;
    }

    int pallet_size = file_header->offbits_ - sizeof(BitmapFileHeader) - sizeof(BitmapInfoHeader);
    int img_size = info_header->size_image_;     // イメージサイズ
    int x = info_header->width_;                 //  イメージの幅
    int y = info_header->heigth_;                // イメージの高さ
    int color_bit = info_header->bit_count_;     // １色あたりのビット数


    //  ヘッダの矛盾を確認
    if (color_bit  != 24) {
        return false;
    }

    if (color_bit  == 0) {
        return false;
    }

    if (x <= 0 || y <= 0) {
        return false;
    }

    if (img_size != file_header->size_ - file_header->offbits_) {
        return false;
    }

    if (img_size != (x * color_bit + 31) / 32 * 4 * y) {
        return false;
    }

    return true;
}


//------------------------------------------------------------------------------------------
//  ビットマップ読み込み. fstream ver.
bool deserializeBmp(
    std::ifstream* file,
    Accessor* accessor
) {

    if(file == nullptr) {
        return false;
    }

    //  ヘッダ取得
    file->read((char *)(&accessor->file_header_), sizeof(BitmapFileHeader));
    if( file->bad() ) {
        //  読み込み失敗
        return nullptr;
    }

    //  ファイルヘッダ
    file->read((char *)(&accessor->info_header_), sizeof(BitmapInfoHeader));
    if( file->bad() ) {
        //  読み込み失敗
        return nullptr;
    }


    if (!isVaildBmp(&accessor->file_header_, &accessor->info_header_)) {
        //  不正なデータ
        return false;
    }

    //  イメージデータ取得
    int img_size = accessor->getImageSize();
    uint8_t* buf = (uint8_t*)malloc(img_size);
    uint8_t* buf_top = buf;

    if (buf == nullptr) {
        //  読み込み失敗
        return false;
    }


    file->read((char *)buf, img_size);

    //  ビットマップからイメージデータ生成
    accessor->data_ = buf;

    //   片付け
    return true;
}


}   // namespace libbmp32


#endif // LIB_BMP32_HPP

