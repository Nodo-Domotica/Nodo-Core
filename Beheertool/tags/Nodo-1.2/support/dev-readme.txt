
* Installeer NetBeans 6.8 of hoger
* Installeer subversion
* Installeer Maven2 (of 3)
* Draai onderstaande commando's vanuit de support folder
* Draai 'mvn assembly:assembly' om de release aan te maken.

------------------------------------------------------------------

N.B. Om een eigen repository te maken, verander de POM met:

  <repositories>
    <repository>
      <id>local</id>
      <url>file://./support/mvn/</url>
    </repository>
  </repositories>

Draai vervolgens de deploy commando's in de vorm van:

  mvn deploy:deploy-file ... -DrepositoryId=local

In het vervolg zullen bovenstaande libraries niet meer van de remote repository geladen worden.

------------------------------------------------------------------

mvn deploy:deploy-file -DrepositoryId=local -Dpackaging=jar -Durl=file://C:/Path/To/support/mvn/ 
		-DgroupId=org.freixas    -DartifactId=jcalendar      -Dversion=20050512 -Dfile=jcalendar.jar
		-DgroupId=gnu.io         -DartifactId=rxtx           -Dversion=2.2pre2  -Dfile=RXTXcomm.jar
		-DgroupId=netbeans       -DartifactId=AbsoluteLayout -Dversion=6.8      -Dfile=AbsoluteLayout.jar
		-DgroupId=com.connectina -DartifactId=FontChooser    -Dversion=unknown  -Dfile=FontChooser.jar
