import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:firebase_database/firebase_database.dart';

class ConfiguracionScreen extends StatefulWidget {
  final bool isAutomaticMode;
  final String id;

  const ConfiguracionScreen({super.key, required this.isAutomaticMode, required this.id});

  @override
  _ConfiguracionScreenState createState() => _ConfiguracionScreenState();
}

class _ConfiguracionScreenState extends State<ConfiguracionScreen> {
  late bool isAutomaticMode;
  String luminosidad = 'Media';
  double temperatura = 25.0;
  double humedad = 50.0;
  bool isCurtainOpen = false;
  late DatabaseReference databaseReference;

  @override
  void initState() {
    super.initState();
    isAutomaticMode = widget.isAutomaticMode;

    // Inicializar la referencia a la base de datos en tiempo real
    databaseReference = FirebaseDatabase.instanceFor(
      app: Firebase.app(),
      databaseURL: 'https://test-ad56c-default-rtdb.firebaseio.com/',
    ).ref().child('Dispositivo');

    // Cargar los valores desde Firestore
    FirebaseFirestore.instance.collection('Dispositivo').doc(widget.id).get().then((document) {
      if (document.exists) {
        setState(() {
          if (document.data()!.containsKey('luminosidad')) {
            luminosidad = document['luminosidad'];
          }
          if (document.data()!.containsKey('temperatura')) {
            temperatura = document['temperatura'];
          }
          if (document.data()!.containsKey('humedad')) {
            humedad = document['humedad'];
          }
          if (document.data()!.containsKey('isCurtainOpen')) {
            isCurtainOpen = document['isCurtainOpen'];
          }
        });
      }
    });
  }

  void _saveSettings() {
    // Actualizar Firestore
    FirebaseFirestore.instance.collection('Dispositivo').doc(widget.id).update({
      'luminosidad': luminosidad,
      'temperatura': temperatura,
      'humedad': humedad,
      'isCurtainOpen': isCurtainOpen,
    });

    // Actualizar Firebase Realtime Database
    databaseReference.update({
      'luminosidad': luminosidad,
      'temperatura': temperatura,
      'humedad': humedad,
      'isCurtainOpen': isCurtainOpen,
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(isAutomaticMode ? 'Configuración - Modo Automático' : 'Configuración - Modo Manual'),
      ),
      body: ListView(
        padding: const EdgeInsets.all(16.0),
        children: [
          if (!isAutomaticMode) ...[
            Card(
              shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(10.0),
              ),
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text('Control Manual', style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
                    RadioListTile<bool>(
                      title: Text('Abrir Cortina'),
                      value: true,
                      groupValue: isCurtainOpen,
                      onChanged: (value) {
                        setState(() {
                          isCurtainOpen = value!;
                        });
                        _saveSettings();
                      },
                    ),
                    RadioListTile<bool>(
                      title: Text('Cerrar Cortina'),
                      value: false,
                      groupValue: isCurtainOpen,
                      onChanged: (value) {
                        setState(() {
                          isCurtainOpen = value!;
                        });
                        _saveSettings();
                      },
                    ),
                  ],
                ),
              ),
            ),
          ] else ...[
            Card(
              shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(10.0),
              ),
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text('Configuración Automática', style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
                    const SizedBox(height: 16),
                    Text('Luminosidad', style: TextStyle(fontSize: 16)),
                    RadioListTile<String>(
                      title: Text('Baja'),
                      value: 'Baja',
                      groupValue: luminosidad,
                      onChanged: (value) {
                        setState(() {
                          luminosidad = value!;
                        });
                        _saveSettings();
                      },
                    ),
                    RadioListTile<String>(
                      title: Text('Media'),
                      value: 'Media',
                      groupValue: luminosidad,
                      onChanged: (value) {
                        setState(() {
                          luminosidad = value!;
                        });
                        _saveSettings();
                      },
                    ),
                    RadioListTile<String>(
                      title: Text('Alta'),
                      value: 'Alta',
                      groupValue: luminosidad,
                      onChanged: (value) {
                        setState(() {
                          luminosidad = value!;
                        });
                        _saveSettings();
                      },
                    ),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 8),
            Card(
              shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(10.0),
              ),
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text('Temperatura', style: TextStyle(fontSize: 16)),
                    Slider(
                      value: temperatura,
                      min: 0,
                      max: 50,
                      divisions: 50,
                      label: temperatura.round().toString(),
                      onChanged: (value) {
                        setState(() {
                          temperatura = value;
                        });
                        _saveSettings();
                      },
                    ),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 8),
            Card(
              shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(10.0),
              ),
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text('Humedad', style: TextStyle(fontSize: 16)),
                    Slider(
                      value: humedad,
                      min: 0,
                      max: 100,
                      divisions: 100,
                      label: humedad.round().toString(),
                      onChanged: (value) {
                        setState(() {
                          humedad = value;
                        });
                        _saveSettings();
                      },
                    ),
                  ],
                ),
              ),
            ),
          ],
        ],
      ),
    );
  }
}
