import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:firebase_database/firebase_database.dart';
import 'configuracion.dart';
import 'visualizacionDatos.dart';

class DetalleCortinaScreen extends StatefulWidget {
  final String id;
  final String name;
  final String code;

  const DetalleCortinaScreen({super.key, required this.id, required this.name, required this.code});

  @override
  _DetalleCortinaScreenState createState() => _DetalleCortinaScreenState();
}

class _DetalleCortinaScreenState extends State<DetalleCortinaScreen> {
  bool isAutomaticMode = true; // Estado inicial del modo
  late DatabaseReference databaseReference;

  @override
  void initState() {
    super.initState();
    // Inicializar la referencia a la base de datos en tiempo real con una URL específica
    databaseReference = FirebaseDatabase.instanceFor(
      app: Firebase.app(),
      databaseURL: 'https://test-ad56c-default-rtdb.firebaseio.com/',
    ).ref().child('Dispositivo');

    // Leer el valor inicial de isAutomaticMode desde Firestore
    FirebaseFirestore.instance.collection('Dispositivo').doc(widget.id).get().then((document) {
      if (document.exists && document.data()!.containsKey('isAutomaticMode')) {
        setState(() {
          isAutomaticMode = document['isAutomaticMode'];
        });
      }
    });
  }

  void _toggleAutomaticMode(bool value) {
    setState(() {
      isAutomaticMode = value;
    });
    // Actualizar el valor de isAutomaticMode en Firestore
    FirebaseFirestore.instance.collection('Dispositivo').doc(widget.id).update({
      'isAutomaticMode': isAutomaticMode,
    });

    // Actualizar el valor de isAutomaticMode en Firebase Realtime Database
    databaseReference.update({
      'isAutomaticMode': isAutomaticMode,
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Detalle de Cortina'),
        leading: IconButton(
          icon: const Icon(Icons.arrow_back),
          onPressed: () {
            Navigator.pop(context);
          },
        ),
      ),
      body: ListView(
        padding: const EdgeInsets.all(16.0),
        children: [
          Card(
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(10.0),
            ),
            child: Padding(
              padding: const EdgeInsets.all(16.0),
              child: Row(
                children: [
                  Icon(Icons.devices, size: 50, color: Colors.blue),
                  const SizedBox(width: 16),
                  Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Text(widget.name, style: const TextStyle(fontSize: 24, fontWeight: FontWeight.bold)),
                      const SizedBox(height: 8),
                      Text('Código: ${widget.code}', style: const TextStyle(fontSize: 16, color: Colors.grey)),
                    ],
                  ),
                ],
              ),
            ),
          ),
          const SizedBox(height: 8), // Reduce el espacio aquí
          Card(
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(10.0),
            ),
            child: ListTile(
              leading: const Icon(Icons.settings_remote, color: Colors.blue),
              title: const Text('Modo Automático/Manual'),
              trailing: Switch(
                value: isAutomaticMode,
                activeColor: Colors.blue,
                onChanged: _toggleAutomaticMode,
              ),
            ),
          ),
          const SizedBox(height: 8), // Reduce el espacio aquí
          Card(
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(10.0),
            ),
            child: ListTile(
              leading: const Icon(Icons.settings, color: Colors.blue),
              title: const Text('Configuración'),
              onTap: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(
                    builder: (context) => ConfiguracionScreen(isAutomaticMode: isAutomaticMode, id: widget.id),
                  ),
                );
              },
            ),
          ),
          const SizedBox(height: 8), // Reduce el espacio aquí
          Card(
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(10.0),
            ),
            child: ListTile(
              leading: const Icon(Icons.bar_chart, color: Colors.blue),
              title: const Text('Visualización de Datos'),
              onTap: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(builder: (context) => const VisualizacionDatosScreen()),
                );
              },
            ),
          ),
          const SizedBox(height: 8), // Reduce el espacio aquí
          Card(
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(10.0),
            ),
            child: ListTile(
              leading: const Icon(Icons.picture_as_pdf, color: Colors.blue),
              title: const Text('Reporte'),
              onTap: () {
                print('Descargando'); // Acción para descargar el reporte
              },
            ),
          ),
        ],
      ),
    );
  }
}
