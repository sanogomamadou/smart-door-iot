---

# 🔐 Système IoT d’Ouverture de Porte Intelligente

Ce projet propose un **système de contrôle d'accès sécurisé** basé sur l’IoT, combinant **authentification par badge RFID et saisie de code via un Keypad**. Il utilise une carte Arduino Uno, une passerelle Python, un broker MQTT, et une interface utilisateur développée sous Node-RED pour offrir une gestion **en temps réel** de l’accès à une porte.

## 📌 Fonctionnalités

- Authentification double : badge RFID ou mot de passe
- Suivi des accès en temps réel via un dashboard web
- Alerte sonore et visuelle en cas d’accès refusé ou d’urgence
- Ouverture de porte à distance depuis le dashboard
- Réinitialisation d'urgence à distance
- Changement du mot de passe Keypad depuis l’interface

---

## 🧠 Architecture du système

1. **Couche embarquée (Arduino)**  
   Gère la lecture du badge RFID, la saisie du code, les LED, le buzzer et le servomoteur.

2. **Passerelle Python (gateway)**  
   Sert de pont entre Arduino (via port série) et le broker MQTT.

3. **Interface Dashboard (Node-RED)**  
   Affiche les logs, permet d’envoyer des commandes et de modifier le mot de passe.

```

\[ RFID / Keypad ] → \[ Arduino ] → \[ Python Gateway ] → \[ MQTT Broker ] → \[ Node-RED Dashboard ]

````

---

## 🧰 Matériel utilisé

- Arduino Uno
- Lecteur RFID MFRC522
- Clavier Keypad 4x4
- Servomoteur SG90
- LEDs, buzzer
- (Optionnel) Module Bluetooth HC-05

### Outils logiciels

- Arduino IDE
- Python 3 avec `pyserial` et `paho-mqtt`
- Node-RED
- Broker MQTT (test.mosquitto.org ou local)
- VS Code + PlatformIO

---

## 🗨️ Topics MQTT utilisés

| Topic                 | Direction    | Description                            |
| --------------------- | ------------ | -------------------------------------- |
| `client_id/telemetry` | Arduino → UI | Envoie les événements (accès, erreurs) |
| `client_id/commands`  | UI → Arduino | Commandes (ouvrir, reset, maj mdp)     |

---

## ✅ Résultats obtenus

* 🔓 Ouverture réussie avec badge autorisé ou bon mot de passe
* 🚨 Alertes en cas de tentative échouée ou d’urgence
* 🌐 Dashboard fonctionnel et réactif
* 🛠️ Mise à jour du mot de passe Keypad testée et validée

---

## 👨‍💻 Membres du projet

* **DIPAMA Andrea Mireille**
* **MENKARI Yahya**
* **SANOGO Mamadou**
* **SOMA Acharaf Mataman Caleb**

---

## 🏷️ Tags

`#IoT` `#SmartDoor` `#Arduino` `#NodeRED` `#Python` `#MQTT` `#ProjetEtudiant` `#TechForGood`
