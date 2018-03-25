# brickBreaker

Descriere

Având la dispoziție o platformă a cărei poziție poate fi modificată deplasând joystick-ul spre stânga sau spre dreapta și o bilă, scopul
acestui joc este spargerea tuturor cărămizilor aflate în partea de sus a matricii de LED-uri. Jocul are 3 nivele, fiecare distingându-se
printr-o dispunere diferită a cărămizilor. Pentru a câștiga, jucătorul trebuie să supraviețuiască tuturor celor 3 nivele. Înainte de a
intra în joc, jucătorul poate seta dificultatea din meniul principal, afișat pe LCD display. Pentru dificultatea 2 sau 3, la trecerea la
un nou nivel, jucătorului i se va adăuga încă o viață la cele rămase la momentul respectiv. La începutul jocului, sunt puse la dispoziție
3 vieți. Se pierde o viață în momentul în care bila ajunge în partea de jos a matricii fără a atinge platforma. La pierderea tuturor celor
3 sau mai multe vieți, jocul se încheie.

Abordare tehnică

Ca hardware, am folosit matricea de LED-uri 8x8, conectată la Arduino prin driver-ul MAX7219, LCD Display-ul cu un potențiometru și un
joystick.
În cod, am folosit 3 clase: brickBall, brickBoard și brickBreaker. În clasa brickBall există o funcție pentru calcularea posibilelor 
coordonate viitoare ale bilei, în funcție de direcție. În clasa brickBoard se află o funcție pentru mișcarea platformei. Toate funcțiile
ce țin de gameplay în rest sunt declarate în clasa brickBreaker. Funcția mainMenu() se ocupă de afișarea meniului principal pe LCD și
navigarea prin acesta, atât timp cât jocul încă nu a început. În momentul începerii jocului, LCD-ul va afișa scorul și viețile, 
inițializate tot în funcția mainMenu(). După ce a fost selectată opțiunea New Game din meniu, jocul este pregătit, matricea afișează primul
nivel și se așteaptă un input stânga/dreapta prin joystick de la jucător pentru începerea jocului. În funcție de acest input, direcția 
bilei este setată la stânga sau la dreapta. Pe parcursul jocului, prin input-ul de la joystick, platforma este deplasată spre stânga sau
spre dreapta și se calculează noile coordonate ale bilei, luând în considerare și posibilele coliziuni cu pereții, platforma sau cărămizile.
Pentru fiecare direcție posibilă a bilei (direcții numerotate de la -3 la 3, cele negative pentru deplasarea în jos, cele pozitive pentru
deplasarea în sus, iar 0 pentru staționare), am luat în calcul toate posibilitățile de coliziune și le-am tratat individual. Funcția
boardRicochet() este apelată în momentul în care bila ajunge pe penultimul rând, ultimul fiind rezervat pentru platformă și are rolul de a
verifica dacă are sau nu loc coliziunea cu platforma, dar și pregătirea pentru generarea unui nou nivel în cazul în care cel curent a fost
terminat sau afișarea mesajului de final în cazul terminării ultimului nivel. Funcția levelGenerate se ocupă de dispunerea cărămizilor în 
cadrul nivelelor 2 și 3.

O previzualizare a gameplay-ului poate fi observata aici: https://drive.google.com/open?id=1mICFTWVoMr5EOT_QVl5ZwKU9qxO4n0pY
