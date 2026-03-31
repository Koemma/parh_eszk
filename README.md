BEADANDÓ

1. feladat (Pthread)
Szürkeárnyalat, monokróm, homályosítás
(grayscale, monochrome, blur)
    Specifikáció:
        - képek átalakítása fekete - fehérre (szürkeárnyalatossá)
        - képek átalakítása egy adott szín árnyalataira (monokróm)
        - képek homályosítása
    
    Futtatások és mérések:
        - szálak számának változtatása (egyszálas és többszálas)
        - kép méretének növelése

        - futási idő mérése
        - skálázhatóság
        - speedup

    Grafikon kiértékelés:
        A grafikonok '#' karakterekkel mutatják mennyi idő alatt hajtották végre az algoritmust, így vizuálisan is látszódik a különbség.

        Speedup grafikon:
        - A speedup grafikon azt mutatja, mennyivel több idő alatt hajtotta végre az algoritmust párhuzamosítva (különböző mennyiségű szálon), mint szekvenciálisan.
        - Egy nagyobb képen (1500x2420 képpont) vizsgáltam a szürkeárnyalatosítást, monokrómosítást (piros, zöld, kék) és a homályosítást.
        - Szürkeárnyalatosítás: Párhuzamosítva egyre gyorsabban hajtja végre, mint egy szálon, ezért ennek jó a skálázhatósága.
        - Piros: Nagyjából ugyan annyi idő alatt hajtja végre, vagy lassabban több szállal.
        - Zöld: 2, 4, és 50 szálon gyorsabb, viszont 16 szálon lassabb. Nem lineárisan skálázódik.
        - Kék: A piroshoz hasonlóan nagyjából egyforma idő alatt megy végbe, de inkább csökken a sebessége, mint nő.
        - Homályosítás: 4 szállal majdnem négyszer olyan gyorsan hajtja végre, mint 1 szállal, viszont utána több szállal már nem sokkal gyorsabban, vagy akár lassabban fut le.

        Fixed thread:
        - A fixed thread grafikon azt mutatja, hogy egy adott, például 4, szálon fut le az algoritmus, különböző képméretekkel.
        - Három képmérettel vizsgáltam a szürkeárnyalatosítást, monokrómosítást (piros, zöld, kék) és a homályosítást.
        - Mindegyik módszerrel a közepes képméretnél jobban skálázódik, mint a kicsinél, viszont nagyobb képnél már nem.