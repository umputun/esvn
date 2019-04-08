<?xml version="1.0" encoding="UTF-8" ?>
<!--
   This stylesheet uses saxon:output to gather a list of figures and screenshots that contain condition="fo" attribute from
   esvn.xml.
   The purpose is to provide a way to remove these images from html output and so reduce distribution payload size.
 
   Author: Sean Wheller sean@inwords.co.za http://www.inwords.co.za
 -->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="text"/>
  <xsl:param name="toppath">html-docs/</xsl:param>
  <xsl:template match="book">
    <xsl:text>rm -f </xsl:text> <xsl:for-each select="//figure[@condition]/screenshot/mediaobject/imageobject/imagedata">
      <xsl:call-template name="script"/>
    </xsl:for-each>
    <xsl:for-each select="//screenshot[@condition]/mediaobject/imageobject/imagedata">
      <xsl:call-template name="script"/>
    </xsl:for-each>
  </xsl:template>
  <xsl:template name="script" match="imagedata">
    <xsl:param name="fileref">
      <xsl:value-of select="@fileref"/>
    </xsl:param>
    <xsl:value-of select="concat($toppath , $fileref)"/>
    <xsl:text> </xsl:text>
  </xsl:template>
</xsl:stylesheet>
