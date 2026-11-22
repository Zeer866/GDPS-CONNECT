#pragma once
#include <Geode/Geode.hpp>
#include <string>
#include <vector>

// Logo del mod incrustado en el codigo (no depende de la carpeta resources).
inline const char* kGcLogoB64 =
    "iVBORw0KGgoAAAANSUhEUgAAAGAAAABgCAMAAADVRocKAAAAwFBMVEWdp58SKycxaTZn7GUjVlQit+lY5q0nk6bL1uYVaoREzvE3"
    "zNqDYo9KrXCK5mm7xtNaZGmE4YQ1i2VDei5tjol2YYOvuMMkOERAPj9AvPyAfn9QjjI9wKOBgH80x/wBAgJ0+JBn+a1G6PWmpaZY"
    "99Da5/s11vyI+W+bmptM9vAcxfxmZWYAAABB2Ps4vPqKiYrp9v53d3h493Y5OTqim6IUFxc74/0oKCoVVnoZaHQweEtk/MVGR0jh"
    "7f0jc2pUVFakCOfiAAAAQHRSTlP//////////////////////////////////////////////////////////wD/////////////"
    "////////////N9U1/AAAB1pJREFUeNq1mgt7mkgUhmcU8JpoXDVtdxeiKEWNgiISo5T//6/2zHCbgRnAdPu1faSxnZdzHy7onRFW"
    "er1rLuXXr1/kD6dPkVqcMLsmyo6UzsHag5aZNkWtqG5Ei1xvsV5AP2O1n1olgKLu95al6zqFxFoy6vdXubTy8kQn0M+U0eIBnT1Z"
    "HJaXrL9cMuuvbmLAW74+6IkB4MNeT7TfSwibRgCW8IxTAD7omawmgFUjQEwgALWvCwlyHy2a+AgCEQN6+4PewIRlIxM4C37+TQCs"
    "gwph3kt9JI0CZ8EzOAm99/ocwZKG+QsAMAFBBGJAX5BH+8fziAe03xE+gHS9QRRkgIU8yuAjpPS/BGjqoxbqlQD7BoTGgL9iQIHw"
    "v/kIoow+qgH7BsVcYQJYQADfGUC/ArCRADiCCMBbYFm/46MCQQSAIMvC3MRHAsD37xmh3+/nBnAnXznW3qRRiAGMCX3d0lWqgToo"
    "qk+lcaoJc+oidiIcfKO5nmpMKAP2S914BHCrNqEE2Hd6vTsjr0bjp6cHAYphmM2FPMPQqghpkNP1DwDw7VzIrpbpGX49IDNg3/H9"
    "yHFs80xVb8DZdH4cW1iRh5kHWB3D2JH/h6gIofL0TYTgXxgGlpvQEgBsdH5ug9AZmfZOLpucCHD8GCAmSAAv7dOpAcAGAPqjAJsF"
    "CAlSFzW1AEkAKUFmwandPqEzLOHIZUOMz0wMhE0VAHpeZhSAIHaQH2aaR/IcJSGGT0MMOAkA+qHTUby73VzmzrsrnxMh4MQAMhOs"
    "5eOtYsFuMfit9qkIAMSHj6Oiksb3zfsG4nrdN2jtLYUqvj6bFC54SgCQavyGWos6gHXRv7BukAgrmsY5qeyinqIca0Wz1MnX97N0"
    "O/r+EwN4KQJIkP26yDo0O0mXywB2/I1tHg1jssgJJYCqKoFH5gBCslFgwpl7ZvoJ9WIGAICi3AWBYzp0yGkSQFLJ0OZNlEhkACyM"
    "YgsiN3RdF4YOKc27YThm/NlOAW8p4FDoRUmrOJdbxW43Ho+jXURjELlrELEAvok870gaSA54ywGFXtSOm50taHa+EbguAiN2UQoA"
    "C8hfji4cBRzg7QsAiOjahZO/H3eMBQSwc9cYMgRFDwBQAXAPgpmD1uHIseEwt8AIvOl0Hq6d6fEeBY0AdGRCDHgALNV1YdGQRNZI"
    "AGFAEjV0w/XadUc0bWsB6AXBb2IBPwHAvaOQnPQ6vPugMQW8wlHQpT925xIAl0VAoCLlwGeoffz3HkR0qS5VfkiPIu9+dDw5gM4D"
    "P5C3f+DRLKKuD9exMZlcNyBBrsgimAeXunlwh3jO5oWlKa8LNe0H4EcfNnsSC+Jmt9tVjnqHZmRJpDh2NulFTEMVAOratWP+oDUl"
    "BAzNIQBuzGATAKxqAGRMBnARmQH3HBB4kLKtyWeVi674VTIDjjklBoQuHQlQ2WECyK97UsAn+tiqal4IVUGG7ukrr2T8ejYFzEcz"
    "DArQfERLAIzDPrWwCFAzC9J5INDZIWdO2jMtWugX4JIhaGYY4zwm3RAJACpbyTBqkkI7czsvm8+eGDAbDsf8z12zDgAni9rPz890"
    "b+r8oE1iCL+OfPa4jAW1gG2T64OIB0hclAAWHGC73Za376fiPIjKLnodzma5i2hkUsCCAWyFgLYA4Ce7nyldcNSdQlkYwYi2OncO"
    "33gBLQUW0BNZIJwHUbEO1mkdhKlr2Ot/BrBNTciC/JIAGMGQ9wLYjdIzzCqZbFmjvJJbyQ5Vy/bBGYC9PsjSlPZn0bZl5mYdOund"
    "MUBj78FkgG0OUK9XJYpqLglkzQ5cNC09WuABSatANfPAdkTzYN1FyMNYkwAyBOxNj86QaXKlbQt241bBTrQwm2gFH5UBB1Wt2ajf"
    "C4OYORzflelUY0xYlADgpUMloJvsKiLSNkmBhV3SQe95/1BulQDd0tOnXD1GHSpM9kVhvi8aUwA1zM32RQpzL08EIJFmxN0QvPo+"
    "6o5IPOcKxkZiAayPlTkCLyE0xXgiAbAMOjpzZfcfLWuADW/tTg2jsxnkALz5xzBgCvlckOUAixd7d/NCvWGXAFCipuvC5pd7yia1"
    "QApYLpUrXMONO8qAB2iTyfi4+zWZFJ7jNQJc+Du0sPCVBCQBuASwWm36UKJaf1UJaOAjIKgEAJ8AGIdQACNiAaxLAKsawFYM4ExY"
    "XjqdwSa2hOySyC6CArROp1+6rV0EbJuYAFpuqAWpKGC1Edw3l1hQCYgpAMCZFO7ZBXvrfwIjs94Aay8giB41l034RK9NLHgUkJvQ"
    "QljdPpiowqflPCBPJIze1QZRvly+CNDIs8yB2iBRv0iYkKexaoMo1AMKBPpoRFtg+jyZM0EWhUuRUW+CdntKnohvmwEulRaUCNpC"
    "w8kzfbU+US9FwmZT66RbK3srQa2vtYcBq5uSv1eB1UFtlAsxqAPcNIV7M+SaIWRzUxDnpXT926qDi++2fKjbQdnrsTYPCfw/weWX"
    "Z8BRrx/X60ema52U0qs7sRQsfDvnD+k/xE8DKx788n8AAAAASUVORK5CYII=";

inline std::vector<unsigned char> gcB64Decode(std::string const& in) {
    static const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<unsigned char> out;
    int val = 0, bits = -8;
    for (unsigned char c : in) {
        auto p = chars.find(static_cast<char>(c));
        if (p == std::string::npos) continue; // ignora '=' y saltos de linea
        val = (val << 6) + static_cast<int>(p);
        bits += 6;
        if (bits >= 0) {
            out.push_back(static_cast<unsigned char>((val >> bits) & 0xFF));
            bits -= 8;
        }
    }
    return out;
}

inline cocos2d::CCSprite* gcLogoSprite() {
    using namespace cocos2d;
    static const char* key = "gdps-connect-logo-embedded";
    auto cache = CCTextureCache::sharedTextureCache();
    CCTexture2D* tex = cache->textureForKey(key);
    if (!tex) {
        auto data = gcB64Decode(kGcLogoB64);
        auto img = new CCImage();
        if (!img->initWithImageData(data.data(), static_cast<int>(data.size()))) {
            img->release();
            return nullptr;
        }
        tex = cache->addUIImage(img, key);
        img->release();
    }
    if (!tex) return nullptr;
    return CCSprite::createWithTexture(tex);
}
