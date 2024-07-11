import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:fl_chart/fl_chart.dart';

class VisualizacionDatosScreen extends StatefulWidget {
  const VisualizacionDatosScreen({super.key});

  @override
  _VisualizacionDatosScreenState createState() => _VisualizacionDatosScreenState();
}

class _VisualizacionDatosScreenState extends State<VisualizacionDatosScreen> {
  List<FlSpot> luzSpots = [];
  List<FlSpot> tempSpots = [];
  List<FlSpot> humedadSpots = [];
  double time = 0;

  @override
  void initState() {
    super.initState();
    _setupFirebaseListeners();
  }

  void _setupFirebaseListeners() {
    final DatabaseReference databaseReference = FirebaseDatabase.instanceFor(
      app: Firebase.app(),
      databaseURL: 'https://test-ad56c-default-rtdb.firebaseio.com/',
    ).ref().child('DispositivoOut');

    databaseReference.child('luz').onValue.listen((event) {
      double luzValue = double.tryParse(event.snapshot.value.toString()) ?? 0;
      setState(() {
        time += 1;
        luzSpots.add(FlSpot(time, luzValue));
        if (luzSpots.length > 20) {
          luzSpots.removeAt(0); // Limitar a los últimos 20 puntos para evitar sobrecargar el gráfico
        }
      });
    });

    databaseReference.child('temp').onValue.listen((event) {
      double tempValue = double.tryParse(event.snapshot.value.toString()) ?? 0;
      setState(() {
        tempSpots.add(FlSpot(time, tempValue));
        if (tempSpots.length > 20) {
          tempSpots.removeAt(0); // Limitar a los últimos 20 puntos para evitar sobrecargar el gráfico
        }
      });
    });

    databaseReference.child('humedad').onValue.listen((event) {
      double humedadValue = double.tryParse(event.snapshot.value.toString()) ?? 0;
      setState(() {
        humedadSpots.add(FlSpot(time, humedadValue));
        if (humedadSpots.length > 20) {
          humedadSpots.removeAt(0); // Limitar a los últimos 20 puntos para evitar sobrecargar el gráfico
        }
      });
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Visualización de Datos'),
      ),
      body: ListView(
        padding: const EdgeInsets.all(16.0),
        children: [
          _buildLineChartCard('Luz', luzSpots, Colors.yellow, 1024),
          _buildLineChartCard('Temperatura', tempSpots, Colors.red, 50),
          _buildLineChartCard('Humedad', humedadSpots, Colors.blue, 100),
        ],
      ),
    );
  }

  Widget _buildLineChartCard(String title, List<FlSpot> spots, Color color, double maxY) {
    return Card(
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(10.0),
      ),
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              title,
              style: const TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
            ),
            const SizedBox(height: 16),
            Center(
              child: AspectRatio(
                aspectRatio: 1.7,
                child: LineChart(
                  LineChartData(
                    gridData: FlGridData(show: true),
                    titlesData: FlTitlesData(show: true),
                    borderData: FlBorderData(show: true),
                    minX: 0,
                    maxX: time,
                    minY: 0,
                    maxY: maxY,
                    lineBarsData: [
                      LineChartBarData(
                        spots: spots,
                        isCurved: true,
                        color: color,
                        barWidth: 4,
                        isStrokeCapRound: true,
                        belowBarData: BarAreaData(
                          show: true,
                          color: color.withOpacity(0.3),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
