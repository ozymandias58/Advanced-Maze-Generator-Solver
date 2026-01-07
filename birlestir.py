import os

# Taranacak dosya uzantıları
uzantilar = [".c", ".h", ".java"]

# Çıktı dosyasının adı
cikti_dosyasi = "tum_kodlar.txt"

def dosyalari_birlestir():
    with open(cikti_dosyasi, "w", encoding="utf-8") as outfile:
        # Bulunduğumuz klasör ve alt klasörleri gez
        for kok_dizin, klasorler, dosyalar in os.walk("."):
            for dosya in dosyalar:
                # Sadece belirlediğimiz uzantılardaki dosyaları al
                if any(dosya.endswith(ext) for ext in uzantilar):
                    dosya_yolu = os.path.join(kok_dizin, dosya)
                    
                    try:
                        with open(dosya_yolu, "r", encoding="utf-8") as infile:
                            icerik = infile.read()
                            
                            # Okunabilirliği artırmak için ayraçlar ve dosya adı ekle
                            outfile.write("\n" + "="*50 + "\n")
                            outfile.write(f"DOSYA ADI: {dosya_yolu}\n")
                            outfile.write("="*50 + "\n\n")
                            outfile.write(icerik + "\n")
                            print(f"Eklendi: {dosya_yolu}")
                    except Exception as e:
                        print(f"Hata oluştu ({dosya_yolu}): {e}")

    print(f"\nİşlem tamam! Tüm kodlar '{cikti_dosyasi}' içine kaydedildi.")

if __name__ == "__main__":
    dosyalari_birlestir()