# Connected-Bee-Hive

Harware

Software

Conception of the system

Schematics

Code

Credits


L’objectif de ce projet est de développer un dispositif complet qui permet de mesurer plusieurs grandeurs physiques et de les rendre disponible à travers une Interface Homme Machine (IHM) sur une application internet. Ce projet doit permettre de récupérer et mettre en forme des données mesurées par des capteurs et ensuite les envoyer sur un serveur (Cloud) grâce à une technologie longue portée et basse consommation (LPWAN).

Spécifications techniques : 
Autonome : Batterie LiPo rechargeable et panneaux solaires comme source d'énergie.
Communication des informations par réseau LPWAN (LoRAWAN) 
Envoi des données toutes les 10 minutes (éventuellement paramétrable) Bouton on/off LED qui s’allume quelques secondes au démarrage du système. 
Affichage des données sous forme graphique sur une application internet : Ubidots STEM pendant la phase de développement et BEEP Monitor (app.beep.nl) pour le rendu final du projet.

Niveau d’exigence forte😫 : 
Poids de la ruche (précision 100 g, résolution 10 g, intervalle entre 0 et 120 Kg). La tare doit être faite en laboratoire.
Température intérieure de la ruche. (Précision 0,5 °C, résolution 0,1 °C, intervalle entre -10 °C et 85 °C) 
État de la batterie exprimée en pourcentage (Précision 1 %, résolution 1 %)
Capteurs d'humidité à l’intérieur de la ruche. (Précision 2%, résolution 0,1 %, intervalle entre 0 et 100%) 

Niveau d’exigence moyen🤗 :
Température extérieure de la ruche, (précision 0,5 °C, résolution 0,1 °C) 
Humidité à l’extérieur de la ruche. (Précision 2%, résolution 1 %) 
Plusieurs capteurs de température à l’intérieure de la ruche. (3 max) 
Détection du vol de la ruche (analyse du poids), envoi des coordonnées approximatives de la ruche obtenue par le réseau LPWAN (si possible). 
Détection de l’essaimage (analyse du poids) 

Niveau d’exigence faible🤪 :
Luminosité extérieure
Changer les paramètres du système depuis l’application web par des downlinks : 
Changer la fréquence d’envoi (1 minutes à 60 minutes par pas de 1 minute) 
Calibration des capteurs (poids, températures, humidité, etc..) 
Calibrer les seuils des alertes 

Alertes 🚨: 
Essaimage 
Poids de la ruche trop bas. 
Poids de la ruche élevé. 
Température trop basse. 
Charge batterie faible. 
Vol de la ruche

Éléments de cadrage :
Pour tous ces projets, le système devra être autonome énergétiquement à l’aide d’une batterie et d’une petite cellule solaire. Il faudra dans un premier temps faire un prototype sur carte de prototypage.
Les données capteurs devront être reporté sur une plateforme de visualisation des données avec un tableau de bord qui permet de voir évaluer les grandeurs dans le temps et éventuellement générer des alarmes (mail ou SMS). Pendant le développement, on utilise la plateforme Ubidots STEM qui est simple d’utilisation, gratuite dans le cadre de l’éducation et propose beaucoup de solution pour faire des tableaux de bords.  Pour le rendu final, il faudra utiliser la plateforme BEEP Monitor (beep.app.nl).
Ensuite il faudra réaliser un circuit électronique (PCB) avec la graveuse et un boitier pour avoir un produit robuste qui puisse être utilisé en extérieur. Une soutenance intermédiaire sera organisée afin de vérifier la fonctionnalité et les caractéristiques du prototype.
