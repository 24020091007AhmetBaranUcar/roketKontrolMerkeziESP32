🚀 ESP32 Roket Fırlatma Kontrol Sistemi (Simülasyon)

Bu proje, gömülü sistemlerin mantığını anlamak ve bir roket fırlatma sürecini donanım-yazılım entegrasyonuyla simüle etmek için geliştirdiğim bir çalışmadır. Sadece "kod yazdım" demek yerine; hafıza optimizasyonu, durum makineleri ve temel fizik formülleri gibi gerçek mühendislik yaklaşımlarını denemeye çalıştım.



🛠 Neler Kullandım?

Donanım: ESP32 (NodeMCU), 2 adet Push Button (Başlat/İptal), 2 adet LED (Durum göstergeleri).



Dil: C / C++ (Arduino IDE üzerinde).



Öne Çıkan Teknikler: \* Finite State Machine (FSM): Kodun birbirine girmemesi için sistemin durumlarını (Standby, Countdown, Liftoff vb.) yöneten bir yapı kurdum.



Union \& Bit-field: Savunma sanayiinde kullanılan hafıza tasarrufu yöntemlerini denemek için sistem bayraklarını (engine\_ready, weather\_clear vb.) sadece 1 byte içinde topladım.



Physics Simulation: Sabit ivme ve yerçekimi formülleriyle roketin irtifa ve hızını saniye saniye hesaplayan küçük bir fizik motoru yazdım.



🕹️ Sistem Nasıl Çalışıyor?

Standby (Bekleme): ESP32 açıldığında sistem "GO" durumunda bekler. Start butonuna basılana kadar hiçbir işlem yapmaz.



Countdown (Geri Sayım): Butona basıldığında 10 saniyelik kritik süreç başlar. Burada sensör verilerini simüle edip limitlerin dışına çıkılıp çıkılmadığını her saniye kontrol ediyorum.



Liftoff (Uçuş): Geri sayım biterse motorlar ateşlenir. 120 saniyelik uçuş başlar. Roket 100.000 metreye (Kármán Hattı) ulaştığında görev başarıyla biter.



Abort (Acil Durum): Geri sayımda veya uçuş sırasında herhangi bir anda "Abort" butonuna basılırsa veya sensörler (sıcaklık, basınç) limiti aşarsa sistem kendini kapatır, güvenli moda geçer ve yeniden başlar.



📁 Dosya Yapısı

rocket\_control.ino: Tüm mantığın döndüğü ana kod dosyası.



README.md: Şu an okuduğunuz proje dokümantasyonu.



💡 Bu Projeden Ne Öğrendim?

İşlemciyi delay() ile uyutmanın acil durum butonlarını nasıl "sağırlaştırdığını" ve sistemin tepki süresini nasıl öldürdüğünü bizzat deneyimledim.



C dilindeki struct ve union yapılarının, veriyi ne kadar düzenli tutabileceğini gördüm.



Bir donanım projesinde "reset" mekanizmasının (yakıtın, hızın sıfırlanması vb.) ne kadar hayati olduğunu fark ettim.



🔨 Devre Şeması (Basitçe)

Start Button: GPIO 12 (Internal Pull-up)



Abort Button: GPIO 14 (Internal Pull-up)



Success LED: GPIO 13



Abort LED: GPIO 27

------------------------------------------------------------------------------------
README FILE IN ENGLISH:

# 🚀 ESP32 Rocket Launch Control System (Simulation)

ESP32-based rocket launch simulator with real-time telemetry 
and safety-critical abort mechanisms.

## Features
- ⏱️ 10-second countdown sequence
- 📊 Real-time sensor monitoring (temp, pressure, fuel)
- 🚨 Automatic abort on critical thresholds
- 🔴 Manual abort button
- 📈 Physics-based flight simulation
- ✨ LED status indicators
- 🎯 Kármán Line (100km) detection

## Hardware
- ESP32 DevKit
- 2x Push buttons (START/ABORT)
- 2x LEDs (Green/Red)
- Breadboard & jumper wires

## Tech Stack
- C/C++ (Arduino framework)
- Embedded systems programming
- State machine design
- Bitwise operations
- Real-time control systems

## Build & Flash
```bash
arduino-cli compile --fqbn esp32:esp32:esp32 .
arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 .
```

## Author
Baran - Aspiring embedded systems engineer

## License
MIT
```
