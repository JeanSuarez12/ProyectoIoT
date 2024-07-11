import 'package:cortinas_iot/firebase_options.dart';
import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'agregarCortina-2.dart'; // Importa la pantalla de agregar cortina
import 'cortinaLista.dart'; // Importa la pantalla de lista de cortinas
import 'detalleCortina.dart'; // Importa la pantalla de detalle de cortina
import 'configuracion.dart';
import 'visualizacionDatos.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp(
    options: DefaultFirebaseOptions.currentPlatform,
  );

  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Cortinas IoT',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: AddCurtainScreen(),
      routes: {
        '/cortinaLista': (context) => CortinaListaScreen(),
        '/agregarCortina': (context) => AgregarCortinaScreen(),
        '/detalleCortina': (context) => DetalleCortinaScreen(id: '', name: '', code: ''), // Inicializa con valores vacíos
        '/configuracion': (context) => const ConfiguracionScreen(isAutomaticMode: true, id: ''),
        '/visualizacionDatos': (context) => const VisualizacionDatosScreen(),
      },
    );
  }
}

class AddCurtainScreen extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Column(
          children: [
            // Flecha de retroceso
            Row(
              children: [
                IconButton(
                  icon: Icon(Icons.arrow_back),
                  onPressed: () {
                    // Acción para regresar a la pantalla anterior
                    Navigator.pop(context);
                  },
                ),
              ],
            ),
            Expanded(
              child: Center(
                child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: <Widget>[
                    // Imagen de la cortina
                    Image.asset('assets/images/cortina_image.png', height: 200),

                    SizedBox(height: 40),

                    // Texto principal
                    const Text(
                      'Agregar Cortina',
                      style: TextStyle(
                        fontSize: 24,
                        fontWeight: FontWeight.bold,
                      ),
                    ),

                    SizedBox(height: 10),

                    // Texto secundario
                    const Text(
                      'Experimente el poder de la organización\nde cortinas con nuestra plataforma.',
                      textAlign: TextAlign.center,
                      style: TextStyle(
                        fontSize: 16,
                        color: Colors.grey,
                      ),
                    ),
                  ],
                ),
              ),
            ),
            // Botón para agregar cortina
            Padding(
              padding: const EdgeInsets.all(16.0),
              child: ElevatedButton(
                onPressed: () {
                  // Navegar a la pantalla de agregar cortina
                  Navigator.push(
                    context,
                    MaterialPageRoute(builder: (context) =>  CortinaListaScreen()),
                  );
                },
                style: ElevatedButton.styleFrom(
                  padding: EdgeInsets.symmetric(vertical: 16),
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(30),
                  ),
                ),
                child: const Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: [
                    Icon(Icons.add),
                    SizedBox(width: 10),
                    Text(
                      'Agregar Cortina',
                      style: TextStyle(
                        fontSize: 18,
                      ),
                    ),
                  ],
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
