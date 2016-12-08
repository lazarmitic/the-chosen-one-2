#include <allegro.h>
#include <math.h>

volatile long counter = 0;
void Increment () 
{ 
	counter++; 
}
END_OF_FUNCTION(Increment)

int main()
{
	allegro_init ();
	install_keyboard();
	set_color_depth(16);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED,900,600,0,0);
	BITMAP *buffer = create_bitmap(900,600);

	LOCK_VARIABLE(counter);
	LOCK_FUNCTION(Increment);
	
	install_int_ex(Increment,BPS_TO_TIMER (35));

	FONT *font1 = load_font("Fonts/font1.pcx",NULL,NULL);

	BITMAP *middleplatform = load_bitmap("Pictures/middleplatform.bmp",NULL);
	BITMAP *player = load_bitmap("Pictures/player.bmp",NULL);
	BITMAP *pozadina = load_bitmap("Pictures/pozadina.bmp",NULL);
	BITMAP *portal = load_bitmap("Pictures/portal.bmp",NULL);
	BITMAP *enemy = load_bitmap("Pictures/enemy.bmp",NULL);
	BITMAP *collectable = load_bitmap("Pictures/collectable.bmp",NULL);
	BITMAP *player2 = load_bitmap("Pictures/player2.bmp",NULL);                                   

	srand(time(NULL)); 

	int genplat = 0; //generator koji odredjuje na kojoj ce platformi da se generise collectable
	const int Gravity = 1; // gravitacija
	int velx = 0 , vely = 0 ; // pomocne kordinate za kretanje playera 1
	int velx2 = 0 , vely2 = 0; // pomocne kordinate za kretanje playera 2                        
	int groundlevel2=135,groundlevel1=285,groundlevel3=385;; // visine zemlje 1 , 2 , 3 
	int collx = 0 , colly = 0; // koordinate collectabla
	float kretanje = 5; // brzina kretanja zavisno od tezine
	bool ptezina = false; // kada resetujemo game proverava se dali je hard mod ili ne
	int tezina = 5,r=0; // promenljive za brzinu kretanja neprijatelja
	bool player2p = false; // da li je ukljucem mod za 2 igraca    
	bool reset1 = false,reset2 = false; // resetovanje jednog od igraca kada pogine
	int x2,y2;
	int goal=1000,goalp=0;

pocetak: // pocetak kada je hardmode iskljucen

	bool pzagenerisanjecol = true,hard = false; // reset game generisanje collectable i postavljanje tezine da nije hard
	
hardpocetak: // pocetak kada je hard mode ukljucen

	int x=250,y=135; // koordinate igraca 1
	if(player2p == true) int x2=265,y2=135; // koordinate igraca 2
	
pocetak4: // vracanje na glavni meni posele biranja broja igraca i tezine
	
	int kmx=375,kmy=205; // koordinate kockice u meniju

	rest(200); // odmor posle vracanja nazad 

	while(!key[KEY_P])
	{
		while(counter > 0)
		{
			blit(pozadina,buffer,0,0,0,0,900,600); //crtanje pozadine u glavnom meniju
			textout_ex(buffer,font1,"The Chosen One",400,100,makecol(139,137,137),-1);//ispisivanje teksta glavnog menija
			textout_ex(buffer,font1,"Start Game",400,200,makecol(139,137,137),-1);//ispisivanje teksta glavnog menija
			textout_ex(buffer,font1,"Difficulty",400,225,makecol(139,137,137),-1);//ispisivanje teksta glavnog menija
			textout_ex(buffer,font1,"Exit",400,250,makecol(139,137,137),-1);//ispisivanje teksta glavnog menija
			textout_ex(buffer,font1,"Number of players",400,275,makecol(139,137,137),-1); //ispisivanje teksta glavnog menija
			rectfill(buffer,kmx,kmy,kmx+10,kmy+10,makecol(100,0,0)); // crtanje kockice za meni

			if(key[KEY_DOWN] && kmy <= 305) { kmy+=25; rest(150); } // pomeranje kockice na dole u meniju 
			if(kmy>280) kmy = 205; // ako je kockica na zadnjoj opciji i pretisnemo na dole treba da se vrati na prvu opciju
			if(key[KEY_UP] && kmy >= 205) { kmy-=25; rest(150); } // pomeranje kockice u meniju na gore
			if(kmy<205) kmy = 280; // ako je kockica na prvoj stavki u meniju i pretisnemo na gore da ide na zadnju stavku

			if(key[KEY_ENTER] && kmy == 205) goto pocetak2; // pokretanje igre
			if(key[KEY_ENTER] && kmy == 230) { kmy+=5; kmy-=25; goto menitezine; } // ulazak u meni biranje tezine
			if(key[KEY_ENTER] && kmy == 255) goto kraj2; // izlaz iz igre
			if(key[KEY_ENTER] && kmy == 280) goto brojigraca; // ulaz u meni za broj igraca

			blit(buffer,screen,0,0,0,0,900,600); // crtanje buffera 
			clear_bitmap(buffer); // brisanje prethodnog buffera
			
			counter--;
		}
	}

menitezine: // labela za meni tezine

	rest(200);

		while(!key[KEY_P])
		{
			while(counter > 0)
			{
				blit(pozadina,buffer,0,0,0,0,900,600); // crtanje buffera
				textout_ex(buffer,font1,"Normal",400,205,makecol(139,137,137),-1); // ispis teksta tezine 
				textout_ex(buffer,font1,"Hard",400,230,makecol(139,137,137),-1); // ispis teksta tezine 
				textout_ex(buffer,font1,"Insane",400,255,makecol(139,137,137),-1); // ispis teksta tezine 

				rectfill(buffer,kmx,kmy,kmx+10,kmy+10,makecol(100,0,0)); // crtanje kockice za meni

				if(key[KEY_DOWN] && kmy <= 285) { kmy+=25; rest(150); } //pomeranje kockice na dole u meniju 
				if(kmy>260) kmy = 210; // ako je kockica na zadnjoj opciji i pretisnemo na dole treba da se vrati na prvu opciju
				if(key[KEY_UP] && kmy >= 210) { kmy-=25; rest(150); } // pomeranje kockice u meniju na gore
				if(kmy<210) kmy = 260; // ako je kockica na prvoj stavki u meniju i pretisnemo na gore da ide na zadnju stavku

				if(key[KEY_ENTER] && kmy == 210) // izabiranje Normal tezine
				{
					tezina = 5; // postavljanje brzine neprijatelja 
					r=1; // oduzimanje brzine neprijatelja sa strane
					ptezina = false; // dali je insein ili ne
					hard = false; // nije insein
					kretanje = 5; // brzina kretanje igraca
					goto pocetak4; // odlazak u glani meni
				}
				if(key[KEY_ENTER] && kmy == 235) // biranje hard tezine
				{
					tezina = 12; // brzina kretanja neprijatelja
					r=3; // oduzimanje od brzine neprijatelja sa strane
					ptezina = false; // nije insein
					hard = false; // nije instein
					kretanje = 5; // brzina kretanja igraca
					goto pocetak4; // odlazak u glavni meni
				}
				if(key[KEY_ENTER] && kmy == 260) // biranje insein tezine
				{
					tezina = 4; // brzina kretanja neprijatelja 
					hard = true; // jeste insein
					ptezina = true; // jeste insein
					kretanje = 3,5; // brzina kretanja igraca 
					r=0; // oduzimanje brzine neprijatelja sa strane
					goto pocetak4; //odlazak u glavni meni
				}

				blit(buffer,screen,0,0,0,0,900,600); //crtanje buffera                                                                
				clear_bitmap(buffer); // brisanje buffera
				counter--;

				if(key[KEY_ESC]) goto pocetak4;// ako pretisnemo esc povratak na glavni meni
			}
		}

brojigraca: // labela za meni broj igraca

	rest(200);

	while(!key[KEY_I])
	{
		while(counter > 0)
		{
			blit(pozadina,buffer,0,0,0,0,900,600); // crtanje pozadine
			textout_ex(buffer,font1,"1 player",400,205,makecol(139,137,137),-1);//ispisivanje teksta menija
			textout_ex(buffer,font1,"2 player",400,230,makecol(139,137,137),-1);//ispisivanje teksta menija
			rectfill(buffer,kmx,kmy,kmx+10,kmy+10,makecol(100,0,0));//crtanje kockice za meni

			if(key[KEY_DOWN] && kmy <= 255) { kmy+=25; rest(150); }//pomeranje kockice na dole u meniju 
			if(kmy>235) kmy = 210;// ako je kockica na zadnjoj opciji i pretisnemo na dole treba da se vrati na prvu opciju
			if(key[KEY_UP] && kmy >= 210) { kmy-=25; rest(150); } // pomeranje kockice u meniju na gore
			if(kmy<210) kmy = 235; // ako je kockica na prvoj stavki u meniju i pretisnemo na gore da ide na zadnju stavku

			if(key[KEY_ENTER] && kmy == 210) { player2p = false; goto pocetak4; } // 1 player izabran povratak na glavni meni
			if(key[KEY_ENTER] && kmy == 235) { player2p = true; x2=265; y2=135; goto goal2player; } // 2 player izabran povratak na glavni meni

			blit(buffer,screen,0,0,0,0,900,600);  // crtanje buffera
			clear_bitmap(buffer); // brisanje buffera

			if(key[KEY_ESC]) goto pocetak4; // ako pretisnemo esc povratak na glavni meni
			counter--;                                             
		}
	}

goal2player:

	rest(200);

	while(!key[KEY_I])
	{
		while(counter > 0)
		{
			blit(pozadina,buffer,0,0,0,0,900,600); // crtanje pozadine
			textout_ex(buffer,font1,"Select how much collections u want for the win:",400,150,makecol(139,137,137),-1);//ispisivanje teksta glavnog menija
			textout_ex(buffer,font1,"10",400,205,makecol(139,137,137),-1);//ispisivanje teksta menija
			textout_ex(buffer,font1,"20",400,230,makecol(139,137,137),-1);//ispisivanje teksta menija
			textout_ex(buffer,font1,"30",400,255,makecol(139,137,137),-1);//ispisivanje teksta menija
			rectfill(buffer,kmx,kmy,kmx+10,kmy+10,makecol(100,0,0));//crtanje kockice za meni

			if(key[KEY_DOWN] && kmy <= 285) { kmy+=25; rest(150); } //pomeranje kockice na dole u meniju 
			if(kmy>260) kmy = 210; // ako je kockica na zadnjoj opciji i pretisnemo na dole treba da se vrati na prvu opciju
			if(key[KEY_UP] && kmy >= 210) { kmy-=25; rest(150); } // pomeranje kockice u meniju na gore
			if(kmy<210) kmy = 260; // ako je kockica na prvoj stavki u meniju i pretisnemo na gore da ide na zadnju stavku

			if(key[KEY_ENTER] && kmy == 210) { goalp = 10; goal = 10; goto pocetak4; } 
			if(key[KEY_ENTER] && kmy == 235) { goalp = 20; goal = 20; goto pocetak4; } 
			if(key[KEY_ENTER] && kmy == 260) { goalp = 30; goal = 30; goto pocetak4; }

			blit(buffer,screen,0,0,0,0,900,600);  // crtanje buffera
			clear_bitmap(buffer); // brisanje buffera

			if(key[KEY_ESC]) goto brojigraca;
			counter--;   
		}
	}

pocetak2: // pocetak kada se resetuje game


	int zivoti = 5; // zivoti igraca 1
	int zivoti2 = 5; // zivoti igraca 2
	int skor=0; // pocetni skor igraca 1
	int skor2=0; // pocetni skor igraca 2
	int en2x = 400,en2y = 135,pomEn2=tezina; //koordinate neprijatelja 2
	int en1x = 100,en1y = 285,pomEn1=tezina-r; // koordinate neprijatelja 1
	int en3x = 800,en3y = 385,pomEn3=tezina-r; // koordinate neprijatelja 3
	int velyen2 = 0; // pomocne koordinate za skakanje neprijatelja 2
	int velyen1 = 0; // pomocne koordinate za skakanje neprijatelja 1
	int velyen3 = 0; // pomocne koordinate za skakanje neprijatelja 3

pocetak3: // pocetak kada se izgubi zivot
	
	if(reset1 == true)
	{
		x=250;
		y=135;
		reset1 = false;
	}
	if(reset2 == true)
	{
		x2=265;
		y2=135;
		reset2 = false;
	}

	while(!key[KEY_I]) // glavna while petlja
	{
		while(counter > 0)
		{
			if(key[KEY_ESC] && ptezina == true) goto hardpocetak; // povratak na glavni meni ako se pretisne esc ako je insein
			if(key[KEY_ESC] && hard == false) goto pocetak; // povratak na glavni meni ako nije insein

			if(key[KEY_P]) goto pause; // pauziranje igre i odlazak u praznu while petlju
			resume: // labela za odpauziranje

			if(hard == true) // ako je insein skakanje neprijatelja
			{
				en2y += velyen2;               //skok neprijatelja 2
				if(en2y == 135) velyen2 = -3;  //skok neprijatelja 2
				if(en2y <= 95) velyen2 =  3;   //skok neprijatelja 2

				en1y += velyen1;               //skok neprijatelja 1
				if(en1y == 285) velyen1 = -3;  //skok neprijatelja 1
				if(en1y <= 245) velyen1 =  3;  //skok neprijatelja 1

				en3y += velyen3;               //skok neprijatelja 3
				if(en3y == 385) velyen3 = -3;  //skok neprijatelja 3
				if(en3y <= 345) velyen3 =  3;  //skok neprijatelja 3
			}

			blit(pozadina,buffer,0,0,0,0,900,600); // crtanje pozadine

			textout_ex(buffer,font1,"Z = jump",0,0,makecol(139,137,137),-1); //uputstvo
			textout_ex(buffer,font1,"R = reset",0,25,makecol(139,137,137),-1); //uputstvo
			textout_ex(buffer,font1,"-> = move right",0,50,makecol(139,137,137),-1); //uputstvo
			textout_ex(buffer,font1,"<- = move left",0,75,makecol(139,137,137),-1); //uputstvo
			textout_ex(buffer,font1,"Jump in pit to win :)",0,100,makecol(139,137,137),-1); //uputstvo

			if(key[KEY_RIGHT]) velx = kretanje;  //movement right player 1 
			else if(key[KEY_LEFT]) velx = -kretanje; //movement left player 1
			else velx = 0; //movement stay player 1
			if(key[KEY_ENTER_PAD] && y == groundlevel2 && x>235 && x<600 || key[KEY_ENTER_PAD] && y == groundlevel1 && x>-15 && x<200 || key[KEY_ENTER_PAD] && y == groundlevel3 && x>635 && x<900  ) vely = -10; //movement jump player 1

			if(player2p == true)
			{
				if(key[KEY_M]) velx2 = kretanje;  //movement right player 2
				else if(key[KEY_N]) velx2 = -kretanje; //movement left player 2
				else velx2 = 0; //movement stay player 2
				if(key[KEY_C] && y2 == groundlevel2 && x2>235 && x2<600 || key[KEY_C] && y2 == groundlevel1 && x2>-15 && x2<200 || key[KEY_C] && y2 == groundlevel3 && x2>635 && x2<900  ) vely2 = -10; //movement jump player 1
			}

			if(vely<10) //ogranicenje gravitacije player 1
			vely += Gravity; //gravitacija player 1
			x += velx; //kretanje player 1
			y += vely; //kretanje player 1

			if(player2p == true)
			{
				if(vely2<10) //ogranicenje gravitacije player 2
				vely2 += Gravity; //gravitacija player 2
				x2 += velx2; //kretanje player 2
				y2 += vely2; //kretanje player 2
			}

			if(y>=135 && y<=150 && x >= 240 && x<=600) y=groundlevel2; //zemlja 2
			if(y>=285 && y<=300 && x >= -15 && x<=199) y=groundlevel1; //zemlja 1
			if(y>=385 && y<=400 && x >= 635 && x<=900) y=groundlevel3; //zemlja 3

			if(player2p == true)
			{
				if(y2>=135 && y2<=150 && x2 >= 240 && x2<=600) y2=groundlevel2; //zemlja 2
				if(y2>=285 && y2<=300 && x2 >= -15 && x2<=199) y2=groundlevel1; //zemlja 1
				if(y2>=385 && y2<=400 && x2 >= 635 && x2<=900) y2=groundlevel3; //zemlja 3
			}

			if(y>585) //pad u rupu igraca 1
			{
				zivoti--; // smanjivanje zivota igraca 1
				reset1 = true;
				goto pocetak3; // odlazak na reset igraca 1
			}

			if(player2p == true)
			{	
				if(y2>585) // pad u rupu igraca 2
				{
					zivoti2--; // gubljenje zivota igraca 2
					reset2 = true;
					goto pocetak3; // odlazak na reset igraca 2
				}
			}

			if(y>150 && x>=235 && x<=400) x=235; // ogranicenje platforme 2 sa leve strane
			if(y>150 && x<=600 && x>=250) x=600; //ogranicenje platforme 2 sa desne strane
			if(y>300 && x<=200 && x>=0) x=200; // ogranicenje platforme 1 sa desne strane
			if(y>400 && x<=885 && x>=635) x=635; //ogranicenje platforme 3 sa leve strane 

			if(player2p == true)
			{
				if(y2>150 && x2>=235 && x2<=400) x2=235; // ogranicenje platforme 2 sa leve strane
				if(y2>150 && x2<=600 && x2>=250) x2=600; //ogranicenje platforme 2 sa desne strane
				if(y2>300 && x2<=200 && x2>=0) x2=200; // ogranicenje platforme 1 sa desne strane
				if(y2>400 && x2<=885 && x2>=635) x2=635; //ogranicenje platforme 3 sa leve strane 
			}
			
			if(x<0) x = 0;     //ogranicenje buffera sa leve strane 
			if(x>=885) x = 885; //ogranicenje buffera sa desne strane 

			if(player2p == true)
			{
				if(x2<0) x2 = 0;     //ogranicenje buffera sa leve strane 
				if(x2>=885) x2 = 885; //ogranicenje buffera sa desne strane 
			}

			if(key[KEY_R]) //reset game
			{
				x=250,y=135; // postavljanje igraca  1  u pocetnu poziciju
				zivoti = 5; // postavljanje zivota na pocetno stanje
				skor = 0; // reset skora na 0

				if(player2p == true)
				{
					x2=265,y2=135; // postavljanje igraca  2  u pocetnu poziciju
					zivoti2 = 5; 
					skor2 = 0;
					goal = goalp;
				}
			}

			en2x = en2x + pomEn2;                                   // pomeranje neprijatelja 2
			if(en2x >= 570) pomEn2 = pomEn2 - pomEn2 - pomEn2;      // pomeranje neprijatelja 2
			if(en2x <= 290) pomEn2 = (pomEn2 * -1) ;                // pomeranje neprijatelja 2

			en1x = en1x + pomEn1;                                   // pomeranje neprijatelja 1
			if(en1x >= 185) pomEn1 = pomEn1 - pomEn1 - pomEn1;      // pomeranje neprijatelja 1
			if(en1x <= 0)   pomEn1 = (pomEn1 * -1);                 // pomeranje neprijatelja 1

			en3x = en3x + pomEn3;                                   // pomeranje neprijatelja 3
			if(en3x >= 890) pomEn3 = pomEn3 - pomEn3 - pomEn3;      // pomeranje neprijatelja 3
			if(en3x <= 650) pomEn3 = (pomEn3 * -1);                 // pomeranje neprijatelja 3
			
			if( x<=en2x+15 && x>=en2x-15 && y>=en2y-15 && y<=en2y+15 ){ zivoti--;  reset1 = true; goto pocetak3;  } // napad neprijatelja 2
			if( x<=en1x+15 && x>=en1x-15 && y>=en1y-15 && y<=en1y+15 ){ zivoti--;  reset1 = true; goto pocetak3;  } // napad neprijatelja 1
			if( x<=en3x+15 && x>=en3x-15 && y>=en3y-15 && y<=en3y+15 ){ zivoti--;  reset1 = true; goto pocetak3;  } // napad neprijatelja 3
				
			if(player2p == true)
			{
				if( x2<=en2x+15 && x2>=en2x-15 && y2>=en2y-15 && y2<=en2y+15 ){ zivoti2--;  reset2 = true; goto pocetak3;  } // napad neprijatelja 2
				if( x2<=en1x+15 && x2>=en1x-15 && y2>=en1y-15 && y2<=en1y+15 ){ zivoti2--;  reset2 = true; goto pocetak3;  } // napad neprijatelja 1
				if( x2<=en3x+15 && x2>=en3x-15 && y2>=en3y-15 && y2<=en3y+15 ){ zivoti2--;  reset2 = true; goto pocetak3;  } // napad neprijatelja 3
			}

			if( x<=collx+10 && x>=collx-15 && y>=colly-15 && y<=colly+10) { pzagenerisanjecol = true; skor++; }   //skupljanje kocke
			
			if(player2p == true)
			{
				if( x2<=collx+10 && x2>=collx-15 && y2>=colly-15 && y2<=colly+10) { pzagenerisanjecol = true; skor2++; }   //skupljanje kocke
			}

			if( pzagenerisanjecol == true ) // generisanje nove collectable
			{
				genplat = rand() % 3 + 1; 
				pzagenerisanjecol = false;

				if(genplat == 2) // generisanje nove collectable plat 2                  
				{                                  
					collx = rand() % 340 + 250;    
					colly = rand() % 30 + 110;                             
				}                                  
				if(genplat == 1)  // generisanje nove collectable plat 1                 
				{
					collx = rand() % 160 + 30;
					colly = rand() % 30 + 260;
				}
				if(genplat == 3) // generisanje nove collectable plat 3
				{
					collx = rand() % 220 + 650;
					colly = rand() % 30 + 360;
				}
			} 
		
			if(x<=5+20 && x>=5-15 && y>=240-15 && y<=240+20) //ulaz u portal levi
			{
				x=350;
				y=135;
			}
			if(x<=875+20 && x>=875-15 && y>=360-15 && y<=360+20) //ulaz u portal desni
			{
				x=550;
				y=135;
			}

			if(player2p == true)
			{
				if(x2<=5+20 && x2>=5-15 && y2>=240-15 && y2<=240+20) //ulaz u portal levi
				{
					x2=350;
					y2=135;
				}
				if(x2<=875+20 && x2>=875-15 && y2>=360-15 && y2<=360+20) //ulaz u portal desni
				{
					x2=550;
					y2=135;
				}
			}

			if(zivoti <= -1) goto kraj; // ako su zivoti nestali igrac 2 winer
			
			if(player2p == true)
			{
				if(zivoti2 <= -1) goto kraj; // ako su zivoti nestali igrac 1 winer
			}

			if(player2p == true) if(skor==goal || skor2 == goal) goto kraj;

			masked_blit(middleplatform,buffer,0,0,250,150,350,450);
			masked_blit(portal,buffer,0,0,875,345,20,20);
			masked_blit(enemy,buffer,0,0,en2x,en2y,15,15);
			masked_blit(enemy,buffer,0,0,en1x,en1y,15,15);
			masked_blit(enemy,buffer,0,0,en3x,en3y,15,15);
			masked_blit(portal,buffer,0,0,5,240,20,20);
			masked_blit(collectable,buffer,0,0,collx,colly,10,10);
			blit(player,buffer,0,0,x,y,15,15);
			if(player2p == true) blit(player2,buffer,0,0,x2,y2,15,15);

			rect(buffer,0,300,200,600,makecol(255,255,255)); // zemlja 1
			rect(buffer,650,400,900,600,makecol(255,255,255)); // zemlja 3
			
			textprintf_ex(buffer,font1,250,3,makecol(139,137,137),-1,"Skor red = %d",skor); //skor 1
			textprintf_ex(buffer,font1,250,25,makecol(139,137,137),-1,"Zivoti red = %d",zivoti); // trenutni zivoti igraca 1
			if(player2p == true) textprintf_ex(buffer,font1,450,3,makecol(139,137,137),-1,"Skor blue = %d",skor2); //skor 2
			if(player2p == true) textprintf_ex(buffer,font1,450,25,makecol(139,137,137),-1,"Zivoti blue = %d",zivoti2); // trenutni zivoti igraca 2

			blit(buffer,screen,0,0,0,0,900,600);
			clear_bitmap(buffer);// brisanje trenutnog buffera da nebi dosto do stackovanje slika
			
			counter--;
	    }
	
	}
	pause:
	while(!key[KEY_U])
	{
		while(counter>0)
		{
			if(key[KEY_U]) goto resume;
			if(key[KEY_ESC] && ptezina == true) goto hardpocetak;
			if(key[KEY_ESC] && hard == false) goto pocetak;
			counter--;
		}
	}
	goto resume;

kraj:

	while(!key[KEY_I]) 
	{
		if(player2p == true)
		{
			if(zivoti == -1 || goal == skor2) textout_ex(screen,font1,"BLUE PLAYER IS WINNER",440,300,makecol(255,0,0),-1);
			if(zivoti2 == -1 || goal == skor) textout_ex(screen,font1,"RED PLAYER IS WINNER",440,300,makecol(255,0,0),-1);
		}

		if(player2p == false) textout_ex(screen,font1,"GAME OVER",440,300,makecol(255,0,0),-1);

		rest(2000);
		if(ptezina == true && hard == true) goto hardpocetak;
		if(ptezina == false && hard == false) goto pocetak;
	}

kraj2:

	destroy_bitmap(middleplatform);
	destroy_bitmap(buffer);
	return 0;
	
	
}
END_OF_MAIN()