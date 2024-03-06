import 'dart:async';
import 'dart:io';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:package_info/package_info.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter App Version Example',
      home: const VersionPage(),
    );
  }
}

class VersionPage extends StatefulWidget {
  const VersionPage({Key? key}) : super(key: key);

  @override
  _VersionPageState createState() => _VersionPageState();
}

class _VersionPageState extends State<VersionPage> {
  late String _version;
  static const platform = MethodChannel('update_manager');

  @override
  void initState() {
    super.initState();
    _initVersion();
  }

  Future<void> _initVersion() async {
    try {
      PackageInfo packageInfo = await PackageInfo.fromPlatform();
      _version = packageInfo.version;
      _checkForUpdate();
    } on PlatformException catch (e) {
      print("Failed to get version: '${e.message}'.");
    }
  }

  Future<void> _checkForUpdate() async {
    try {
      await platform.invokeMethod('checkForUpdate', {"version": _version});
    } on PlatformException catch (e) {
      print("Failed to check for update: '${e.message}'.");
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('App Version'),
      ),
      body: Center(
        child: Text(
          'Version: $_version',
          style: Theme.of(context).textTheme.headline5,
        ),
      ),
    );
  }
}
