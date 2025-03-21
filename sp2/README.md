V rámci tohoto zadání bude vaším úkolem implementovat jednoduchou knihovnu pro
práci s tzv. multiple precision celými čísly. Jak jistě víte, běžně ukládaná 
čísla v dnešních zařízeních obvykle při své největší délce respektují omezení 
ve formě velikosti strojového registru (na x86_64 tedy 8bajtů/64bitů). 

Samozřejmě existují výjimky, jako je například kryptografie - šifrovací klíče 
jsou obvykle čísla daleko větší, než jen 64 bitů. Tyto datové typy je obvykle 
nutné "emulovat" na vyšší úrovni (pokud není procesor vybaven 
specializovaným hardware).

V rámci tohoto zadání bude vaším úkolem naprogramovat přesně tuhle emulaci 
čísel s větší, než je základní přesnost. Přesněji - emulaci čísel, které mohou 
být tak velká, jak jim jen hlavní paměť dovolí. V rámci zadání operujeme 
s pojmem neomezená přesnost, fakticky ale jsme samozřejmě omezeni 
velikostí hlavní paměti.

# Zadání
Vytvořte třídu reprezentující celé číslo s neomezenou přesností. 
Vnitřní reprezentaci volte tak, aby bylo možné provádět nad těmito čísly různé 
operace co nejefektivněji. Tato třída bude šablonová - bude parametrizována 
číslem, které bude buď odpovídat maximální velikosti čísla v bajtech 
(vždy >= 4; omezujeme tedy přesnost shora), nebo vybranou konstantou 
naznačí skutečnost, že požadujeme skutečně neomezenou přesnost.

Instancovat pak lze například:
MPInt<12> cislo;
MPInt<MPInt::Unlimited> neomezene_cislo;

Nad těmito čísly dovolte provádět operace 
 - sčítání (binární +), 
 - odčítání (binární -), 
 - násobení (binární *), 
 - dělení (binární /) a 
 - faktoriál (unární, zleva asociativní !). 

Pro tyto operace přetěžte všechny dostupné odpovídající operátory včetně 
složených přiřazovacích (+,-,*,/,+=,-=,*=,/=), pro faktoriál vytvořte regulérní 
členskou metodu. Tyto operace bude možné provádět i nad různě šablonově 
parametrizovanými vstupy! Výsledná přesnost (výše uvednená "maximální velikost") 
bude volena jako největší ze vstupních (např. sčítáme-li číslo s 
přesností 12 a 16, výsledné číslo bude mít velikost 16. Podobně pak 
násobíme-li číslo s přesností 60 a neomezenou, výsledné číslo bude mít 
neomezenou přesnost). Výsledná přesnost operace faktoriál bude shodná s 
přesností operandu. Rovněž budou mít čísla korektně implementovanou copy a move 
sémantiku (vč. copy- a move- assignment operátorů).

Pokud se výsledek operace do čísla s danou přesností nevejde (např. by došlo k 
přetečení), vyhoďte výjimku a původní čísla nijak nemodifikujte. Místo toho vraťte 
přetečené číslo jako atribut výjimky (tzn. budete muset vytvořit vlastní třídu pro 
výjimku). Přetečeným číslem se rozumí takové číslo, které by bylo výsledkem 
operace za předpokladu, že neošetřujeme přetečení (pouze ho dovolujeme/tolerujeme).

Vše výše uvedené implementujte v podobě hlavičkové knihovny (tedy jeden soubor .h 
nebo .hpp, který obsahuje veškerou implementaci, popř. sada .h/.hpp souborů s 
jedním centrálním - cílem je, aby se dal includovat jeden soubor a vše fungovalo).

Nad touto knihovnou vytvořte textové konzolové rozhraní, jehož režim rovněž půjde 
šablonově parametrizovat, které bude přejímat jednoduché operace v příkazové 
řádce a na konzoli bude vypisovat jejich výsledek. Posledních 5 výstupů dovolte 
ukládat jako číslo se zadanou přesností (tento požadavek je nadřazený výše 
uvedenému - tzn. zde každé číslo "zdegeneruje" do přesnosti zadané jako šablonový 
parametr tohoto rozhraní) a dovolte na ně odkazovat zástupnými symboly $1 až $5 
($1 nejnovější, $5 nejstarší). Každé další číslo "posune" číslování, klasicky 
tak, jak byste očekávali (tzn. nový výsledek bude $1, původní $1 se posune na 
$2, ... a původní $5 se zahodí). Místo čísla tedy půjde zadat tento zástupný znak. 
Dovolte vypsat tuto historii příkazem bank. Dále implementujte příkaz exit, 
kterým se konzolové rozhraní ukončí.

Terminál bude nejspíš implementován v rámci třídy, a tak půjde instancovat 
nejspíše například takto:
MPTerm<66> omezeny_terminal;
MPTerm<MPInt::Unlimited> neomezeny_terminal;

Následně nad tímto vším vytvořte obal, který bude umět spouštět tři režimy:

konzolové rozhraní s neomezenou přesností
konzolové rozhraní s omezenou přesností na 32 bajtů
demonstrační kód, kde implementujete nějaký "showcase" toho, co Vaše knihovna umí 
 - demonstrujte vše z výše uvedených požadavků na generickou knihovnu
Program tedy půjde spustit z příkazové řádky jako

./mpcalc.exe <mode>

Kde <mode> je číslo 1, 2 nebo 3 podle výše uvedeného klíče. Samozřejmě ošetřete, 
zda je zadán tento parametr správně, a v případě, že ne, vypište nápovědu.

Průběh pak může vypadat například pro příkazovou řádku:

./mpcalc.exe 1

Takto:

MPCalc - rezim s neomezenou presnosti
Zadejte jednoduchy matematicky vyraz s nejvyse jednou operaci +, -, *, / nebo !
> 100+100
$1 = 200
> 5+1+1
Neplatne zadani
> $1+$1
$1 = 400
> bank
$1 = 400
$2 = 200
> 100!
$1 = 93326215443944152681699238856266700490715968264381621468592963895217599993229915608941463976156518286253697920827223758251185210916864000000000000000000000000
> exit
Koncim.

Poznámky
Není nijak omezeno, jak máte vnitřně čislo reprezentovat - stačí, když dodržíte 
zadaná omezení. Je samozřejmě vhodné volit binární reprezentaci s rozumně efektivními 
operacemi nad ní, ovšem pokud se vám zdá, že je to na vás "příliš", je možné číslo 
reprezentovat i jako řetězec a operace provádět algoritmem "jako na základní škole". 
Výkonnost pak samozřejmě bude horší. Bajtové omezení se pak samozřejmě vztahuje na 
počet cifer ve zvolené soustavě (nejspíš dekadická).

Dodatečné požadavky
K výše uvedeným požadavkům musí navíc program splnit následující kritéria:

musí být přeložitelný
na Windows (MSVS2022) i na Linuxu (gcc/clang)
k semestrální práci dodáte CMakeLists.txt soubor pro přeložení pomocí nástroje CMake
nesmí padat (segfault, abort, ...)
nesmí vykazovat úniky paměti (bude ověřeno nástroji valgrind a CrtDebug)
musí projít statickou analýzou MSVS2022

Dále se v implementaci musí objevit:
rozumný objektový návrh 
  - členění do tříd, dědičnost, správná viditelnost atributů, dodržování zvyklostí a coding standardů
copy a move sémantika
přetížené operátory
lambda funkce
použité RAII (chytré ukazatele nebo kontejnery)
regulární výrazy
výjimky
šablony
koncept
rozumné množství compile-time omezení a ověření (např. pomocí static_assert, std::enable_if, ...)
optimalizace kódu 
  - správné dekorování konstant a metod pomocí 
  const, final, použití správných datových struktur, algoritmů z knihovny <algorithm>, apod.
tomuto kroku věnujte pak čas třeba i na konci, až budete celý program procházet před odevzdáním
