import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Timer',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: const TimerPage(title: 'Flutter Timer'),
    );
  }
}

class TimerPage extends StatefulWidget {
  const TimerPage({Key? key, required this.title}) : super(key: key);

  final String title;

  @override
  _TimerPageState createState() => _TimerPageState();
}

class _TimerPageState extends State<TimerPage> {
  late Timer _timer;
  int _secondsElapsed = 0;

  @override
  void initState() {
    super.initState();
    _startTimer();
    // Check for update when the app starts
    UpdateManager.checkForUpdate();
  }

  void _startTimer() {
    const oneSecond = Duration(seconds: 1);
    _timer = Timer.periodic(oneSecond, (Timer timer) {
      setState(() {
        _secondsElapsed++;
      });
    });
  }

  void _stopTimer() {
    _timer.cancel();
  }

  void _resetTimer() {
    setState(() {
      _secondsElapsed = 0;
    });
  }

  @override
  Widget build(BuildContext context) {
    String timerText = formatTimer(_secondsElapsed);

    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            Text(
              'Elapsed Time:',
            ),
            Text(
              timerText,
              style: Theme.of(context).textTheme.headline4,
            ),
          ],
        ),
      ),
      floatingActionButton: Column(
        mainAxisAlignment: MainAxisAlignment.end,
        children: [
          FloatingActionButton(
            onPressed: _stopTimer,
            tooltip: 'Stop Timer',
            child: const Icon(Icons.stop),
          ),
          const SizedBox(height: 16),
          FloatingActionButton(
            onPressed: _resetTimer,
            tooltip: 'Reset Timer',
            child: const Icon(Icons.refresh),
          ),
        ],
      ),
    );
  }

  String formatTimer(int seconds) {
    Duration duration = Duration(seconds: seconds);
    String twoDigits(int n) => n.toString().padLeft(2, "0");
    String twoDigitMinutes = twoDigits(duration.inMinutes.remainder(60));
    String twoDigitSeconds = twoDigits(duration.inSeconds.remainder(60));
    return "${twoDigits(duration.inHours)}:$twoDigitMinutes:$twoDigitSeconds";
  }

  @override
  void dispose() {
    _timer.cancel();
    super.dispose();
  }
}

class UpdateManager {
  static const MethodChannel _channel = MethodChannel('update_manager');

  static Future<void> checkForUpdate() async {
    try {
      await _channel.invokeMethod('checkForUpdate');
    } on PlatformException catch (e) {
      print("Failed to check for update: '${e.message}'.");
    }
  }
}
