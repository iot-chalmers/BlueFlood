<?xml version="1.0" encoding="UTF-8"?>
<simconf>
  <project EXPORT="discard">[APPS_DIR]/mrm</project>
  <project EXPORT="discard">[APPS_DIR]/mspsim</project>
  <project EXPORT="discard">[APPS_DIR]/avrora</project>
  <project EXPORT="discard">[APPS_DIR]/serial_socket</project>
  <project EXPORT="discard">[APPS_DIR]/collect-view</project>
  <project EXPORT="discard">[APPS_DIR]/powertracker</project>
  <simulation>
    <title>My simulation</title>
    <randomseed>1234590</randomseed>
    <motedelay_us>1000000</motedelay_us>
    <radiomedium>
      org.contikios.mrm.MRM
      <obstacles />
    </radiomedium>
    <events>
      <logoutput>40000</logoutput>
    </events>
    <motetype>
      org.contikios.cooja.mspmote.SkyMoteType
      <identifier>sky1</identifier>
      <description>Sky Mote Type #sky1</description>
      <firmware EXPORT="copy">[CONTIKI_DIR]/apps/chaos/max/max-app.cooja06.sky</firmware>
      <moteinterface>org.contikios.cooja.interfaces.Position</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.RimeAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.IPAddress</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.Mote2MoteRelations</moteinterface>
      <moteinterface>org.contikios.cooja.interfaces.MoteAttributes</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspClock</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspMoteID</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyButton</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyFlash</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyCoffeeFilesystem</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.Msp802154Radio</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspSerial</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyLED</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.MspDebugOutput</moteinterface>
      <moteinterface>org.contikios.cooja.mspmote.interfaces.SkyTemperature</moteinterface>
    </motetype>
<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>0.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>118</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>0.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>209</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>0.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>119</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>30.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>217</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>30.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>224</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>30.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>206</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>60.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>210</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>60.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>117</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>60.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>207</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>90.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>226</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>90.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>222</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>90.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>100</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>120.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>101</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>120.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>102</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>120.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>103</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>150.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>104</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>150.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>105</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>150.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>106</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>180.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>107</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>180.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>108</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>180.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>109</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>210.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>110</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>210.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>111</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>210.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>112</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>240.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>113</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>240.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>114</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>240.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>115</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>270.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>116</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>270.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>150</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>270.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>151</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>300.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>152</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>300.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>153</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>300.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>200</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>330.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>201</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>330.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>202</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>330.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>203</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>360.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>204</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>360.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>205</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>360.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>208</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>390.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>211</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>390.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>212</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>390.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>213</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>420.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>214</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>420.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>215</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>420.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>216</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>450.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>218</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>450.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>219</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>450.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>220</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>480.000000</x>
    <y>0.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>221</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>480.000000</x>
    <y>30.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>223</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>


<mote>
  <breakpoints />
  <interface_config>
    org.contikios.cooja.interfaces.Position
    <x>480.000000</x>
    <y>60.000000</y>
    <z>0.0</z>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspClock
    <deviation>1.0</deviation>
  </interface_config>
  <interface_config>
    org.contikios.cooja.mspmote.interfaces.MspMoteID
    <id>225</id>
  </interface_config>
  <motetype_identifier>sky1</motetype_identifier>
</mote>  
</simulation>
  <plugin>
    org.contikios.cooja.plugins.SimControl
    <width>188</width>
    <z>2</z>
    <height>175</height>
    <location_x>2</location_x>
    <location_y>279</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.Visualizer
    <plugin_config>
      <moterelations>true</moterelations>
      <skin>org.contikios.cooja.plugins.skins.IDVisualizerSkin</skin>
      <skin>org.contikios.cooja.plugins.skins.GridVisualizerSkin</skin>
      <viewport>0.696436789182562 0.0 0.0 0.696436789182562 49.59448960128138 128.48024239749938</viewport>
    </plugin_config>
    <width>186</width>
    <z>3</z>
    <height>278</height>
    <location_x>1</location_x>
    <location_y>1</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.LogListener
    <plugin_config>
      <filter>ID:14</filter>
      <formatted_time />
      <coloring />
      <hidedebug />
    </plugin_config>
    <width>769</width>
    <z>0</z>
    <height>453</height>
    <location_x>189</location_x>
    <location_y>0</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.TimeLine
    <plugin_config>
      <mote>0</mote>
      <mote>1</mote>
      <mote>2</mote>
      <mote>3</mote>
      <mote>4</mote>
      <mote>5</mote>
      <mote>6</mote>
      <mote>7</mote>
      <mote>8</mote>
      <mote>9</mote>
      <mote>10</mote>
      <mote>11</mote>
      <mote>12</mote>
      <mote>13</mote>
      <mote>14</mote>
      <mote>15</mote>
      <showRadioRXTX />
      <showRadioChannels />
      <showRadioHW />
      <showLEDs />
      <zoomfactor>50.0</zoomfactor>
    </plugin_config>
    <width>959</width>
    <z>1</z>
    <height>288</height>
    <location_x>-1</location_x>
    <location_y>454</location_y>
  </plugin>
</simconf>

