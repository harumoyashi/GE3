#include "NMtllib.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "NTexture.h"

NMaterial NMtllib::Load(const std::string& directoryPath, const std::string& filename)
{
    NMtllib data;
    NMaterial material;
    material.texture = NTextureManager::GetInstance()->CreateErrorTexture();

    //ファイルストリーム
    std::ifstream file;
    //マテリアル情報書いてるファイルを開く
    file.open((directoryPath + filename));
    //ファイル開けなかったらエラー
    if (file.fail()) {
        assert(0);
    }

    //一行ずつ読み込む
    std::string line;
    while (getline(file, line)) {
        //1行分の文字列をストリームに変換
        std::istringstream line_stream(line);

        //半角スペース区切りで行の先頭文字列を取得
        std::string key;
        getline(line_stream, key, ' ');

        //先頭のタブ文字は無視する
        if (key[0] == '\t') {
            key.erase(key.begin()); //先頭の文字を削除
        }
        //先頭の文字がnewmtlならマテリアル名
        if (key == "newmtl") {
            //マテリアル名読み込み
            line_stream >> material.name;
        }
        //先頭の文字がKaならアンビエント色(環境光)
        if (key == "Ka") {
            line_stream >> material.ambient.x;
            line_stream >> material.ambient.y;
            line_stream >> material.ambient.z;
        }
        //先頭の文字がKdならディフューズ色(拡散反射光)
        if (key == "Kd") {
            line_stream >> material.diffuse.x;
            line_stream >> material.diffuse.y;
            line_stream >> material.diffuse.z;
        }
        //先頭の文字がKsならスペキュラー色(鏡面反射光)
        if (key == "Ks") {
            line_stream >> material.specular.x;
            line_stream >> material.specular.y;
            line_stream >> material.specular.z;
        }
        //先頭の文字がmap_Kdならテクスチャファイル名
        if (key == "map_Kd") {
            //テクスチャのファイル名読み込み
            line_stream >> material.textureFilename;
            std::string texFileName = (directoryPath + material.textureFilename);
            material.texture = NTextureManager::GetInstance()->LoadTexture(texFileName, material.textureFilename);
        }
    }
    file.close();

    //最後にちゃんと読み込んだ情報シェーダーに流して
    material.TransferLight();
    return material;
}
