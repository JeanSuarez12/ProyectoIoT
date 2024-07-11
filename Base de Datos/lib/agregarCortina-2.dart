import 'package:flutter/material.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'cortinaLista.dart'; // Importa la pantalla de Cortina Lista

class AgregarCortinaScreen extends StatefulWidget {
  const AgregarCortinaScreen({super.key});

  @override
  _AgregarCortinaScreenState createState() => _AgregarCortinaScreenState();
}

class _AgregarCortinaScreenState extends State<AgregarCortinaScreen> {
  final TextEditingController nameController = TextEditingController();
  final TextEditingController codeController = TextEditingController();

  @override
  void dispose() {
    nameController.dispose();
    codeController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Agregar Cortina'),
        leading: IconButton(
          icon: const Icon(Icons.arrow_back),
          onPressed: () {
            Navigator.pop(context);
          },
        ),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: <Widget>[
            const Text(
              'Ingresa la información de tu nueva cortina',
              style: TextStyle(fontSize: 16, color: Colors.grey),
            ),
            const SizedBox(height: 20),
            const Text(
              'Nombre de la Cortina',
              style: TextStyle(fontSize: 14, color: Colors.black),
            ),
            TextField(
              controller: nameController,
              decoration: InputDecoration(
                hintText: 'Nombre',
                border: OutlineInputBorder(
                  borderRadius: BorderRadius.circular(8),
                ),
              ),
            ),
            const SizedBox(height: 20),
            const Text(
              'Código del Dispositivo',
              style: TextStyle(fontSize: 14, color: Colors.black),
            ),
            TextField(
              controller: codeController,
              decoration: InputDecoration(
                hintText: 'Código',
                border: OutlineInputBorder(
                  borderRadius: BorderRadius.circular(8),
                ),
              ),
            ),
            const Spacer(),
            ElevatedButton(
              onPressed: () async {
                // Obtener los valores de los campos
                String name = nameController.text;
                String code = codeController.text;

                // Referencia a Firestore
                FirebaseFirestore firestore = FirebaseFirestore.instance;

                try {
                  // Guardar el nombre y el código en un nuevo documento en la colección Dispositivo
                  await firestore.collection('Dispositivo').add({
                    'name': name,
                    'code': code,
                  });

                  // Navegar a la pantalla cortinaLista
                  Navigator.push(
                    context,
                    MaterialPageRoute(builder: (context) => const CortinaListaScreen()),
                  );
                } catch (e) {
                  print('Error: $e');
                  // Manejar el error aquí, tal vez mostrando un mensaje al usuario
                  ScaffoldMessenger.of(context).showSnackBar(
                      SnackBar(content: Text('Error: $e'))
                  );
                }
              },
              style: ElevatedButton.styleFrom(
                padding: const EdgeInsets.symmetric(vertical: 16),
                shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(30),
                ),
              ),
              child: const Center(
                child: Text(
                  'Agregar Cortina',
                  style: TextStyle(fontSize: 18),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
