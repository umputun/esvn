<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    <!-- This file is a customization layer for HTML only -->
    <!-- ======================= -->
    <!-- Imports -->
    <!-- RH -->
    <xsl:import href="/usr/share/xml/docbook/xsl-stylesheets/html/profile-chunk.xsl"/>
    <!-- SuSE 9.2 -->
    <!--<xsl:import href="/usr/share/xml/docbook/stylesheet/nwalsh/current/html/profile-chunk.xsl"/>-->
    <xsl:include href="common-cust.xsl"/>
    
    <!-- ======================= -->
    <!-- Parameters -->
    <!-- ======================= -->
    <xsl:param name="profile.condition" select="'html'"/>
    <xsl:param name="base.dir" select="'../dist/html-docs/'"/>
    <xsl:param name="generate.legalnotice.link" select="1"/>
    <xsl:param name="chunker.output.indent" select="'yes'"/>
    <xsl:param name="body.font.master" select="12"/>
    <xsl:param name="html.stylesheet" select="'corpstyle.css'"/>
    <xsl:param name="navig.graphics" select="1"/>
    <xsl:param name="navig.graphics.path" select="'common/navig/'"/>
    <xsl:param name="navig.graphics.extension" select="'.gif'"/>
    <xsl:param name="navig.showtitles" select="1"/>
    
   <!-- Admon Graphics -->
    <xsl:param name="admon.graphics" select="1"/>
    <xsl:param name="admon.textlabel" select="0"/>
    <xsl:param name="admon.graphics.path" select="'common/admon/'"/>
    <xsl:param name="admon.graphics.extension" select="'.png'"/>
    
    <!-- Callout Graphics -->
    <xsl:param name="callout.unicode" select="1"/>
    <xsl:param name="callout.graphics" select="0"/>
    <xsl:param name="callout.graphics.path" select="'common/callouts/'"/>
    <xsl:param name="callout.graphics.extension" select="'.png'"/>
  
    <!-- ======================= -->
    <!-- Attribute Sets -->
    <!-- ======================= -->
    
    <!-- ======================= -->
    <!-- Template Customizations -->
    <!-- ======================= -->
    
</xsl:stylesheet>
