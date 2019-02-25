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
    <x>2.000000</x>
    <y>-21.000000</y>
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
    <x>-11.000000</x>
    <y>70.000000</y>
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
    <x>-5.000000</x>
    <y>114.000000</y>
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
    <x>44.000000</x>
    <y>16.000000</y>
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
    <x>40.000000</x>
    <y>41.000000</y>
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
    <x>62.000000</x>
    <y>124.000000</y>
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
    <x>107.000000</x>
    <y>-25.000000</y>
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
    <x>105.000000</x>
    <y>67.000000</y>
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
    <x>116.000000</x>
    <y>85.000000</y>
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
    <x>158.000000</x>
    <y>-16.000000</y>
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
    <x>168.000000</x>
    <y>54.000000</y>
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
    <x>126.000000</x>
    <y>86.000000</y>
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
    <x>211.000000</x>
    <y>-19.000000</y>
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
    <x>205.000000</x>
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
    <x>224.000000</x>
    <y>102.000000</y>
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
    <x>228.000000</x>
    <y>14.000000</y>
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
    <x>229.000000</x>
    <y>70.000000</y>
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
    <x>274.000000</x>
    <y>97.000000</y>
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
    <x>287.000000</x>
    <y>4.000000</y>
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
    <x>276.000000</x>
    <y>41.000000</y>
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
    <x>277.000000</x>
    <y>107.000000</y>
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
    <x>348.000000</x>
    <y>-16.000000</y>
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
    <x>351.000000</x>
    <y>27.000000</y>
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
    <x>343.000000</x>
    <y>113.000000</y>
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
    <x>423.000000</x>
    <y>22.000000</y>
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
    <x>405.000000</x>
    <y>28.000000</y>
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
    <x>408.000000</x>
    <y>123.000000</y>
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
    <x>434.000000</x>
    <y>-15.000000</y>
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
    <x>430.000000</x>
    <y>28.000000</y>
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
    <x>447.000000</x>
    <y>117.000000</y>
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
    <x>485.000000</x>
    <y>-8.000000</y>
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
    <x>485.000000</x>
    <y>46.000000</y>
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
    <x>504.000000</x>
    <y>98.000000</y>
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
    <x>527.000000</x>
    <y>-7.000000</y>
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
    <x>552.000000</x>
    <y>27.000000</y>
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
    <x>557.000000</x>
    <y>87.000000</y>
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
    <x>608.000000</x>
    <y>-11.000000</y>
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
    <x>595.000000</x>
    <y>68.000000</y>
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
    <x>591.000000</x>
    <y>88.000000</y>
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
    <x>660.000000</x>
    <y>-24.000000</y>
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
    <x>675.000000</x>
    <y>35.000000</y>
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
    <x>654.000000</x>
    <y>98.000000</y>
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
    <x>694.000000</x>
    <y>20.000000</y>
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
    <x>707.000000</x>
    <y>40.000000</y>
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
    <x>710.000000</x>
    <y>106.000000</y>
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
    <x>729.000000</x>
    <y>-23.000000</y>
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
    <x>769.000000</x>
    <y>71.000000</y>
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
    <x>759.000000</x>
    <y>85.000000</y>
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
    <x>825.000000</x>
    <y>-16.000000</y>
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
    <x>781.000000</x>
    <y>73.000000</y>
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
    <x>822.000000</x>
    <y>102.000000</y>
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
    <z>3</z>
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
      <viewport>0.5803639909854685 0.0 0.0 0.5803639909854685 -51.28907797226559 55.211422477916095</viewport>
    </plugin_config>
    <width>186</width>
    <z>0</z>
    <height>278</height>
    <location_x>1</location_x>
    <location_y>1</location_y>
  </plugin>
  <plugin>
    org.contikios.cooja.plugins.LogListener
    <plugin_config>
      <filter>ID:118</filter>
      <formatted_time />
      <coloring />
      <hidedebug />
    </plugin_config>
    <width>769</width>
    <z>2</z>
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
      <mote>16</mote>
      <mote>17</mote>
      <mote>18</mote>
      <mote>19</mote>
      <mote>20</mote>
      <mote>21</mote>
      <mote>22</mote>
      <mote>23</mote>
      <mote>24</mote>
      <mote>25</mote>
      <mote>26</mote>
      <mote>27</mote>
      <mote>28</mote>
      <mote>29</mote>
      <mote>30</mote>
      <mote>31</mote>
      <mote>32</mote>
      <mote>33</mote>
      <mote>34</mote>
      <mote>35</mote>
      <mote>36</mote>
      <mote>37</mote>
      <mote>38</mote>
      <mote>39</mote>
      <mote>40</mote>
      <mote>41</mote>
      <mote>42</mote>
      <mote>43</mote>
      <mote>44</mote>
      <mote>45</mote>
      <mote>46</mote>
      <mote>47</mote>
      <mote>48</mote>
      <mote>49</mote>
      <mote>50</mote>
      <showRadioRXTX />
      <showRadioChannels />
      <showRadioHW />
      <showLEDs />
      <zoomfactor>5000.0</zoomfactor>
    </plugin_config>
    <width>959</width>
    <z>1</z>
    <height>676</height>
    <location_x>-1</location_x>
    <location_y>456</location_y>
  </plugin>
</simconf>

