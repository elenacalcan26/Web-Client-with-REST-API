# Web-Client-with-REST-API

<h5>Calcan Elena-Claudia <br/>
321CA</h5><br/>

  Programul reprezinta implementarea unui client web care interactioneaza cu 
un server gata implementat ce reprezinta o biblioteca online. 
	Implementarea programului a pornit de la rezolvarea Laboratorului 10 si 
folosind sheletul sau de cod.
	Pentru parsarea in format JSON am folosit biblioteaca din fisierele parson.c
si parson.h pentru o parsare corecta si rapida a unui JSON; aceste fisiere au 
fost preluate din urmatorul link: https://github.com/kgabis/parson

  Clientul accepta comenzi de la tastatura si trimite cereri catre server. 
	Comunicarea dintre client si server se realizeaza print-un socket care se
	deschide dupa ce clientul primeste comanda si se inchide atunci cand comanda
	s-a efectuat. <br/><br/>


### 1. register
-------------------------------------------------------------------
	
  • efectueaza inregistrarea unui client <br/>
	• se citesc de la stdin username-ul si parola si se creeaza un payload JSON
	cu acestea <br/>
	• se creeaza o cerere POST care este trimisa catre server <br/>
	• se verifica raspunsul de la server <br/>
	• daca cererea a fost realizata cu succes se afiseaza la stdout mesajul:

    "[SUCCESS] Register", urmat de codul si textul statusului

  • in caz contrar se va afisa mesajul

    "[ERROR] Can't register", urmat de cosul si textul statusului si sectiunea
		de DATE din raspuns
    
<br/>


### 2. login
---------------------------------------------------------------------

  • efectueaza autentificarea unui client <br/>
	• la inceput se verifica daca exista deja un client logat <br/>
	• daca un client este deja logat se afiseaza urmatorul mesaj <br/>

		"Already logged in!"

  • se citesc de la tastatura username-ul si parola si se creeaza un payload
	JSON cu acestea <br/>
	• se creeaza o cerere de tip POST si se trimite catre server <br/>
	• este preluat raspunsul de la server si se verifica daca cererea s-a realizat
	cu succes <br/>
	• daca cererea s-a realizat cu succes, se salveaza cookie-ul de logare si se 
	afiseaza urmatorul mesaj <br/>

		"[SUCCESS] Login", urmat de codul si textul status si cookie-ul pe urmatoarea linie

  • in caz contrare se afiseaza mesajul:

		"[ERROR] Can't login ", urmat de codul si textul status si sectiunea de DATE din raspuns 

<br/>

### 3. enter_library
------------------------------------------------------------------------

  • efectueaza cererea de acces la biblioteca <br/>
	• la inceput se verifica daca clientul este logat si deja are autorizatie <br/>
	• daca clientul nu este logat se va afisa urmatorul mesaj <br/>

		"You are not logged in!" 

  • daca clientul are deja autorizatie se afiseaza mesajul

		"You already have authorization!"

  • se creeaza o cerere de tip GET si se trimite catre server <br/>
	• este preluat raspusnul de la server si se verifica daca s-a realizat cu succes <br/>
	• daca cererea s-a realizat cu succes, se salveaza Token-ul JWT si se afiseaza mesajul <br/>

		"[SUCCESS] You got authorization to the library", urmat de codul si textul statut si
		token-ul JWT pe urmatoarea linie

  • in caz contrar se afiseaza mesajul

		"[ERROR] Can't enter library", urmat de codul si textul status

<br/>

### 4. get_books
----------------------------------------------------------------------

  • efectueaza cerea tuturor cartilor din biblioteca unui client <br/>
	• la inceput se verifica daca clientul este logat si are autorizatie <br/>
	• daca clientul nu este logat, se afiseaza mesajul:

		"You are not logged in!"

  • daca clientul nu are autorizatie, se afiseaza mesajul:

		"You don't have authorization to library!" 

  • se creeaza o cerere de tip GET si se trimite catre server <br/>
	• raspunusul server-ului este preluat si se verifica daca s-a realizat cu succes <br/>
	• daca s-a realizat cu succes, se afiseaza mesajul

		"[SUCCESS] The books in your library are", urmat de codul si textul status,
		pe urmatoare linie se afiseaz lista cu informatii sumare despre toate cartile
		clientului, acestea fiind extrase din sectiunea de DATE din raspunsul primit
		de la server

  • in caz contrar, se afiseaza meajul

		"[ERROR] Can't get books", urmat de codul si text status, iar pe urmatoarea linie 
		este afisata sectiunea de DATE din rapsunusl primit de la server

<br/>

### 5. get_book
----------------------------------------------------------------------

  • efectueaza afisarea tuturor informatiilor despre o carte cu un anumit ID <br/>
	• la inceput se verifica daca clientul este logat si are autorizatie <br/>
	• daca clientul nu este logat, se afiseaza mesajul:

		"You are not logged in!"

  • daca clientul nu are autorizatie, se afiseaza mesajul:

		"You don't have authorization to library!" 

  • se citeste de la stdin ID-ul cartii dorite <br/>
	• se creeaza o cerere de tip GET si se trimite catre server <br/>
	• raspunsul de la server este preluat si se verifica daca cererea s-a efectuat
	cu succes <br/>
	• daca s-a realizat cu succes, se afiseaza mesajul

		"[SUCCES] Got the book with ID = id_citit_de_la_stdin", urmat de codul si
		text statusul, iar pe urmatoarea linie sunt afisate informatiile cartii care 
		sunt extrase din sectiunea de DATE din raspunsul server-ului

  • in caz contrar, se afiseaza mesajul

		"[ERROR] Can't get book with ID = id_citit_stdin", urmat de cod si text status,
		iar pe urmatoarea linie sa afiseaza sectiunea de DATE

<br/>

### 6. add_book
----------------------------------------------------------------------

  • realizeaza adaugarea unei carti in biblioteca unui client <br/>
	• la inceput se verifica daca clientul este logat si are autorizatie <br/>
	• daca clientul nu este logat, se afiseaza mesajul:

		"You are not logged in!"

  • daca clientul nu are autorizatie, se afiseaza mesajul:

		"You don't have authorization to library!" 

  • se citesc pe rand de la stdin: titlul, autorul, genul, editorul si numarul de 
	pagini si se creeaza un payload JSON <br/>
	• se creeaza o cerere de tip POST si se trimite catre server <br/>
	• raspunsul de la server este preluat si se verifica daca cererea s-a realizat cu succes <br/>
	• daca s-a realizat cu succes, se afiseaza mesajul

		"[SUCCESS] The book is added", urmat de cod si text status

  • in caz contrar, este afisat mesajul

		"[ERROR] Can't add book", urmat de cod si text status, iar pe urmatoarea
		linie este afisata sectiunea de DATE din raspuns

<br/>

### 7. delete_book
----------------------------------------------------------------------

  • realizeaza stergearea unei carti cu un anumit id din biblioteca clientului <br/>
	• la inceput se verifica daca clientul este logat si are autorizatie <br/>
	• daca clientul nu este logat, se afiseaza mesajul:

		"You are not logged in!"

  • daca clientul nu are autorizatie, se afiseaza mesajul:

		"You don't have authorization to library!" 

  • se citeste de la stdin ID-ul cartii dorite <br/> 
	• se creeaza o cerere de tip DELETE si se trimite catre server <br/>
	• raspunsul de la server este preluat si se verifica daca a fost realizat
	cu succes <br/>
	• daca s-a realizat cu succes, este afisat mesajul

		"[SUCCESS] The book with ID = id_citit_stdin is deleted", urmat de cod si text status

  • in caz contrar, se afiseaza mesajul

		"[ERROR] Can't delete book with ID = id_citit_stdin", urmat de cod si text status,
		iar pe urmatoarea linie este afisata sectiunea de DATE din raspuns

<br/>

### 8. logout
------------------------------------------------------------------------- 

  • realizeaza logout-ul unui client <br/>
	• se verifica daca clientul este logat sau nu <br/>
	• daca nu este logat, se afiseaza mesajul

		"You are not logged in!"

  • se creeaza o cerere de tip GET si se trimite catre server <br/>
	• raspunsul server-ului este preluat si se verifica daca cererea s-a realizat
	cu succes <br/>
	• daca s-a realizat cu succes, se afiseaza mesajul

		"[SUCCESS] You are logged out", urmat de cod si text status

  • in caz contrar, se afiseaza mesajul

		"[ERROR] Can't logout", urmat de cod si text status, iar pe urmatoarea
		linie se afiseaza sectiunea de DATE a raspunsului

<br/>

### 9. exit
------------------------------------------------------------------
	
  • efectueaza inchiderea programului	
