/**
 * Test Signal - Génération 50% ou Copie de Signal
 * 
 * Ce projet peut :
 * 1. Générer un signal PWM avec 50% de duty cycle sur GPIO 2
 * 2. Copier un signal d'entrée (GPIO 21) vers la sortie (GPIO 2)
 * 
 * Caractéristiques du signal généré (mode 1):
 * - Fréquence: 50 Hz (période 20 ms)
 * - Duty Cycle: 50% (10 ms HIGH, 10 ms LOW)
 * - Tension: 3.3V (niveau logique ESP32)
 * 
 * Mode copie (mode 2):
 * - Lit le signal sur GPIO 21 (entrée)
 * - Copie le signal sur GPIO 2 (sortie) avec le même duty cycle
 * - Utilise PWM hardware pour génération précise
 * 
 * Auteur: Mohamed EL JILY
 */

// ============================================================================
// INCLUDES
// ============================================================================
#include <ESP32Servo.h>

// ============================================================================
// CONFIGURATION
// ============================================================================
#define INPUT_PIN 21        // GPIO 21 - Entrée du signal (pour copie)
#define OUTPUT_PIN 2        // GPIO 2 - Sortie du signal
#define PERIOD_US 20000      // 20 ms (50 Hz)
#define HIGH_TIME_US 10000  // 10 ms (50% de 20 ms)
#define LOW_TIME_US 10000   // 10 ms (50% de 20 ms)

// ============================================================================
// VARIABLES DE MODE
// ============================================================================
bool testModeGenerate = false;   // true = génération 50%, false = copie signal
bool testModeCopy = true;      // true = copie signal, false = génération 50%

// Variables pour copie de signal
unsigned long lastMeasuredPulseWidth = 0;
unsigned long lastMeasuredPeriod = 0;
// PWM hardware pour génération précise du duty cycle
ESP32PWM pwmOutput;
const int pwmResolution = 16;  // 16 bits pour précision
bool pwmInitialized = false;

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(9600);
  delay(500);
  
  // Configurer les pins
  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(OUTPUT_PIN, LOW);
  pinMode(INPUT_PIN, INPUT);  // Pin d'entrée pour copie
  
  Serial.println("========================================");
  if (testModeGenerate) {
    Serial.println("TEST MODE: Signal Generation 50%");
    Serial.print("Output Pin: GPIO ");
    Serial.println(OUTPUT_PIN);
    Serial.print("Frequency: 50 Hz (period ");
    Serial.print(PERIOD_US / 1000);
    Serial.println(" ms)");
    Serial.print("Duty Cycle: 50% (");
    Serial.print(HIGH_TIME_US / 1000);
    Serial.print(" ms HIGH, ");
    Serial.print(LOW_TIME_US / 1000);
    Serial.println(" ms LOW)");
  } else if (testModeCopy) {
    Serial.println("TEST MODE: Signal Copy");
    Serial.print("Input Pin: GPIO ");
    Serial.println(INPUT_PIN);
    Serial.print("Output Pin: GPIO ");
    Serial.println(OUTPUT_PIN);
    Serial.println("Connect generator to GPIO 21");
    // Initialiser PWM hardware pour copie précise
    float initialFreq = 1000000.0 / (float)PERIOD_US;  // Fréquence initiale (50 Hz)
    pwmOutput.attachPin(OUTPUT_PIN, initialFreq, pwmResolution);
    pwmInitialized = true;
  }
  Serial.println("Connect oscilloscope to GPIO 2");
  Serial.println("========================================");
}

// ============================================================================
// FONCTION: GÉNÉRATION SIGNAL 50%
// ============================================================================
void handleTestGenerate() {
  // Générer le pulse HIGH
  digitalWrite(OUTPUT_PIN, HIGH);
  delayMicroseconds(HIGH_TIME_US);
  
  // Générer le pulse LOW
  digitalWrite(OUTPUT_PIN, LOW);
  delayMicroseconds(LOW_TIME_US);
  
  // Afficher les informations toutes les secondes
  static unsigned long lastDisplayTime = 0;
  if (millis() - lastDisplayTime >= 1000) {
    lastDisplayTime = millis();
    Serial.print("Signal active - Frequency: 50 Hz, Duty: 50%");
    Serial.println();
  }
}

// ============================================================================
// FONCTION: COPIE SIGNAL
// ============================================================================
void handleTestCopy() {
  // Mesurer la période complète du signal pour calculer le duty cycle
  // Mesurer d'abord le pulse HIGH
  unsigned long pulseWidthHigh = pulseIn(INPUT_PIN, HIGH, 25000);
  
  if (pulseWidthHigh > 0) {
    // Mesurer ensuite le pulse LOW pour obtenir la période complète
    unsigned long pulseWidthLow = pulseIn(INPUT_PIN, LOW, 25000);
    
    if (pulseWidthLow > 0) {
      // Calculer la période totale
      unsigned long periodTotal = pulseWidthHigh + pulseWidthLow;
      
      // Calculer le duty cycle réel : (pulse HIGH / période totale) * 100
      float dutyCyclePercent = (float)pulseWidthHigh / (float)periodTotal * 100.0;
      
      // Stocker pour affichage
      lastMeasuredPulseWidth = pulseWidthHigh;
      lastMeasuredPeriod = periodTotal;
      
      // Calculer la fréquence réelle du signal d'entrée
      float frequency = 1000000.0 / (float)periodTotal;
      
      // Mettre à jour le PWM hardware avec la fréquence et le duty cycle mesurés
      if (pwmInitialized) {
        // Ajuster la fréquence si elle a changé
        pwmOutput.adjustFrequency(frequency, dutyCyclePercent / 100.0);
      } else {
        // Initialiser le PWM si pas encore fait
        pwmOutput.attachPin(OUTPUT_PIN, frequency, pwmResolution);
        pwmInitialized = true;
      }
      
      // Calculer la valeur PWM pour le duty cycle (0 à 2^16 - 1 = 65535)
      unsigned long maxDutyValue = (1UL << pwmResolution) - 1;
      unsigned long pwmDutyValue = (unsigned long)((dutyCyclePercent / 100.0) * (float)maxDutyValue);
      
      // Écrire le duty cycle sur le PWM
      pwmOutput.write(pwmDutyValue);
    }
  } else {
    // Aucun signal détecté
    static unsigned long lastWarningTime = 0;
    if (millis() - lastWarningTime >= 2000) {
      lastWarningTime = millis();
      Serial.println("WARNING: No signal detected on GPIO 21");
    }
  }
  
  // Afficher les informations toutes les secondes
  static unsigned long lastDisplayTime = 0;
  if (millis() - lastDisplayTime >= 1000) {
    lastDisplayTime = millis();
    
    if (lastMeasuredPulseWidth > 0 && lastMeasuredPeriod > 0) {
      float dutyCycle = (float)lastMeasuredPulseWidth / (float)lastMeasuredPeriod * 100.0;
      float periodMs = (float)lastMeasuredPeriod / 1000.0;
      float frequency = 1000000.0 / (float)lastMeasuredPeriod;
      
      // Calculer la valeur PWM utilisée
      unsigned long maxDutyValue = (1UL << pwmResolution) - 1;
      unsigned long pwmDutyValue = (unsigned long)((dutyCycle / 100.0) * (float)maxDutyValue);
      
      Serial.println("========================================");
      Serial.println("TEST: Signal Copy (PWM Hardware)");
      Serial.println("----------------------------------------");
      Serial.println("INPUT (GPIO 21) - Measured:");
      Serial.print("  Pulse Width HIGH: ");
      Serial.print(lastMeasuredPulseWidth);
      Serial.println(" µs");
      Serial.print("  Period Total:     ");
      Serial.print(lastMeasuredPeriod);
      Serial.println(" µs");
      Serial.print("  Duty Cycle:       ");
      Serial.print(dutyCycle, 2);
      Serial.println(" %");
      Serial.print("  Frequency:        ");
      Serial.print(frequency, 2);
      Serial.println(" Hz");
      Serial.println("----------------------------------------");
      Serial.println("OUTPUT (GPIO 2) - Generated:");
      Serial.print("  Duty Cycle:       ");
      Serial.print(dutyCycle, 2);
      Serial.println(" % (same as input)");
      Serial.print("  Frequency:        ");
      Serial.print(frequency, 2);
      Serial.println(" Hz (same as input)");
      Serial.print("  PWM Duty Value:   ");
      Serial.print(pwmDutyValue);
      Serial.print(" / ");
      Serial.println(maxDutyValue);
      Serial.println("----------------------------------------");
      Serial.println("Check oscilloscope on GPIO 2");
      Serial.println("========================================");
    }
  }
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
  // Basculer entre les deux modes selon les variables
  if (testModeGenerate) {
    handleTestGenerate();
  } else if (testModeCopy) {
    handleTestCopy();
  }
}

