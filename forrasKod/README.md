BEADANDÓ

A beadandóm témája egy OpenCL alapú képfeldolgozó rendszer, amely különböző szűrőket alkalmaz PPM formátumú képeken. A lényege, hogy a képpontok feldolgozását a GPU végzi, így a műveletek párhuzamosan futnak, ami jelentős gyorsulást eredményez a CPU hoz képest. A program öt különböző szűrőt valósít meg, grayscale, red, green, blue és blur. Mindegyik külön OpenCL kernelként működik, és a képpontok RGB értékeit módosítja.


Specifikáció:
    
    grayscale:

        A grayscale szűrő célja, hogy a bemeneti RGB képet szürkeárnyalatossá alakítsa. A művelet minden pixel esetén a három színcsatorna súlyozott átlagát számítja ki.

    monochrome:

        A szűrők egy adott színcsatornát kiemelnek, a többit lenullázzák.

    blur:

        Minden pixel környezetében egy ablakot jár be, és a szomszédos pixelek átlagát adja vissza.
    

Futtatások és mérések:

    képek skálázása: 

        A szűrők alkalmazása különböző méretű képeken. 

        A képek mérete a következő: 
        kep01.ppm - 8000 x 12907,
        kep02.ppm - 4000 x 4368,
        kep03.ppm - 500 x 588 pixel.

    futási idő mérése:

        CPU futási idő:
        A gryascale és blur szűrőknél CPU-n is végrehajtottam a műveletet.
        GPU futási idő:
        Visszaadja, hogy mennyi ideig futott egy adott kernel a GPU-n.

    speedup:

        Megmutatja, hogy a GPU hányszor gyorsabb, mint a CPU a művelet elvégzésében.


