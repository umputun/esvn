<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
    
    <!-- This file is a common customization layer -->
    <!-- ======================= -->
    
    <!-- ======================= -->
    <!-- Parameters -->
    <!-- ======================= -->
    
    <!-- Extensions -->
    <xsl:param name="use.extensions" select="1"/>
    <xsl:param name="saxon.extensions" select="1"/>
    <xsl:param name="tablecolumns.extension" select="1"/>
    <xsl:param name="callouts.extensions" select="1"/>
    
    <!-- General Formatting-->
    <xsl:param name="draft.mode" select="'no'"/>
    <xsl:param name="variablelist.as.blocks" select="1"/>
    <xsl:param name="shade.verbatim" select="1"/>
    <xsl:param name="hyphenate">false</xsl:param>
    <xsl:param name="alignment">left</xsl:param>
    
    <!-- Cross References -->
    <xsl:param name="insert.xref.page.number" select="1"/>
    <xsl:param name="xref.with.number.and.title" select="0"/>
    
    <!-- Indexes -->
    <xsl:param name="generate.index" select="0"/>
    
    <!-- Glossaries -->
    <xsl:param name="glossterm.auto.link" select="1"/>
    <xsl:param name="firstterm.only.link" select="1"/>
    <xsl:param name="glossary.collection">../common/glossary.xml</xsl:param>
    <xsl:param name="glossentry.show.acronym" select="'primary'"/>
    
    <!-- Captions -->
    <xsl:param name="formal.title.placement">
        figure before
        example before 
        equation before 
        table before 
        procedure before
    </xsl:param>
    
    <!-- ======================= -->
    <!-- Templates -->
    <!-- ======================= -->

    <!-- Inline Formatting -->
    <xsl:template match="filename">
      <xsl:call-template name="inline.boldmonoseq"/>
    </xsl:template>
    <xsl:template match="application">
      <xsl:call-template name="inline.boldseq"/>
    </xsl:template>
    <xsl:template match="guibutton">
      <xsl:call-template name="inline.boldseq"/>
    </xsl:template>
    <xsl:template match="guilabel">
      <xsl:call-template name="inline.italicseq"/>
    </xsl:template>
    
</xsl:stylesheet>
