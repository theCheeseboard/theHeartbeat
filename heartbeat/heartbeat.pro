#-------------------------------------------------
#
# Project created by QtCreator 2019-01-07T23:48:41
#
#-------------------------------------------------

QT       += core gui x11extras thelib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = theheartbeat
TEMPLATE = app

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += x11
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    processes/process.cpp \
    processes/processmanager.cpp \
    processes/processmodel.cpp \
    system/systemmanager.cpp \
    panes/percentagepane.cpp \
    panes/minmaxpane.cpp \
    processaction.cpp \
    panes/minipercentagepane.cpp \
    aboutdialog.cpp

HEADERS += \
        mainwindow.h \
    processes/process.h \
    processes/processmanager.h \
    processes/processmodel.h \
    system/systemmanager.h \
    panes/percentagepane.h \
    panes/minmaxpane.h \
    processaction.h \
    panes/minipercentagepane.h \
    aboutdialog.h

FORMS += \
        mainwindow.ui \
    panes/percentagepane.ui \
    panes/minmaxpane.ui \
    processaction.ui \
    panes/minipercentagepane.ui \
    aboutdialog.ui


TRANSLATIONS += \
    translations/au_AU.ts \
    translations/ab_GE.ts \
    translations/aa_DJ.ts \
    translations/aa_ER.ts \
    translations/aa_ET.ts \
    translations/af_ZA.ts \
    translations/sq_AL.ts \
    translations/am_ET.ts \
    translations/ar_DZ.ts \
    translations/ar_BH.ts \
    translations/ar_TD.ts \
    translations/ar_KM.ts \
    translations/ar_DJ.ts \
    translations/ar_EG.ts \
    translations/ar_ER.ts \
    translations/ar_IQ.ts \
    translations/ar_JO.ts \
    translations/ar_KW.ts \
    translations/ar_LB.ts \
    translations/ar_LY.ts \
    translations/ar_MR.ts \
    translations/ar_MA.ts \
    translations/ar_OM.ts \
    translations/ar_PS.ts \
    translations/ar_QA.ts \
    translations/ar_SA.ts \
    translations/ar_SO.ts \
    translations/ar_SD.ts \
    translations/ar_SY.ts \
    translations/ar_TZ.ts \
    translations/ar_TN.ts \
    translations/ar_AE.ts \
    translations/ar_YE.ts \
    translations/hy_AM.ts \
    translations/as_IN.ts \
    translations/az_AZ.ts \
    translations/be_BY.ts \
    translations/bn_BD.ts \
    translations/bi_VU.ts \
    translations/bs_BA.ts \
    translations/bg_BG.ts \
    translations/my_MM.ts \
    translations/ca_AD.ts \
    translations/zh_CN.ts \
    translations/zh_SG.ts \
    translations/cr_CA.ts \
    translations/hr_HR.ts \
    translations/cs_CZ.ts \
    translations/da_DK.ts \
    translations/dv_MV.ts \
    translations/nl_NL.ts \
    translations/nl_BE.ts \
    translations/dz_BT.ts \
    translations/en_AU.ts \
    translations/en_US.ts \
    translations/en_GB.ts \
    translations/en_NZ.ts \
    translations/en_CA.ts \
    translations/eo.ts    \
    translations/et_EE.ts \
    translations/fj_FJ.ts \
    translations/fi_FI.ts \
    translations/fr_CA.ts \
    translations/fr_FR.ts \
    translations/ka_GE.ts \
    translations/de_DE.ts \
    translations/de_AT.ts \
    translations/de_CH.ts \
    translations/el_GR.ts \
    translations/gn_PY.ts \
    translations/ht_HT.ts \
    translations/ha_NE.ts \
    translations/he_IL.ts \
    translations/hi_IN.ts \
    translations/ho_PG.ts \
    translations/hu_HU.ts \
    translations/id_ID.ts \
    translations/ga_IE.ts \
    translations/ig_NG.ts \
    translations/is_IS.ts \
    translations/it_IT.ts \
    translations/iu_CA.ts \
    translations/ja_JP.ts \
    translations/jv_IN.ts \
    translations/kk_KZ.ts \
    translations/km_KH.ts \
    translations/ki_KE.ts \
    translations/rw_RW.ts \
    translations/ky_KG.ts \
    translations/ko_KR.ts \
    translations/ku_IR.ts \
    translations/kj_AO.ts \
    translations/kj_NA.ts \
    translations/lb_LU.ts \
    translations/lg_UG.ts \
    translations/ln_CD.ts \
    translations/lo_LA.ts \
    translations/lt_LT.ts \
    translations/lv_LV.ts \
    translations/mk_MK.ts \
    translations/mg_MG.ts \
    translations/ms_MY.ts \
    translations/ms_SG.ts \
    translations/mt_MT.ts \
    translations/mi_NZ.ts \
    translations/mn_MN.ts \
    translations/ne_NP.ts \
    translations/nb_NO.ts \
    translations/nn_NO.ts \
    translations/pa_PK.ts \
    translations/pa_IN.ts \
    translations/fa_IR.ts \
    translations/pl_PL.ts \
    translations/pt_AO.ts \
    translations/pt_BR.ts \
    translations/pt_CV.ts \
    translations/pt_TL.ts \
    translations/pt_GQ.ts \
    translations/pt_GW.ts \
    translations/pt_MZ.ts \
    translations/pt_PT.ts \
    translations/pt_ST.ts \
    translations/rn_BI.ts \
    translations/ro_RO.ts \
    translations/ro_MD.ts \
    translations/ru_RU.ts \
    translations/ru_BY.ts \
    translations/ru_KZ.ts \
    translations/ru_KG.ts \
    translations/sm_WS.ts \
    translations/sg_CF.ts \
    translations/sr_RS.ts \
    translations/sr_BA.ts \
    translations/si_LK.ts \
    translations/sk_SK.ts \
    translations/sl_SI.ts \
    translations/so_SO.ts \
    translations/st_LS.ts \
    translations/es_ES.ts \
    translations/es_CL.ts \
    translations/es_AR.ts \
    translations/es_BO.ts \
    translations/es_CO.ts \
    translations/es_DO.ts \
    translations/es_EC.ts \
    translations/es_SV.ts \
    translations/es_GT.ts \
    translations/es_HN.ts \
    translations/es_MX.ts \
    translations/es_NI.ts \
    translations/es_PA.ts \
    translations/es_PY.ts \
    translations/es_PE.ts \
    translations/es_UY.ts \
    translations/es_VE.ts \
    translations/su_SD.ts \
    translations/sv_SE.ts \
    translations/th_TH.ts \
    translations/tk_TM.ts \
    translations/tl_PH.ts \
    translations/to_TO.ts \
    translations/tr_TR.ts \
    translations/uk_UA.ts \
    translations/ur_PK.ts \
    translations/uz_UZ.ts \
    translations/ve_ZA.ts \
    translations/vi_VN.ts \
    translations/cy_GB.ts \
    translations/zu_ZA.ts

qtPrepareTool(LUPDATE, lupdate)
genlang.commands = "$$LUPDATE -no-obsolete -source-language en_US $$_PRO_FILE_"

qtPrepareTool(LRELEASE, lrelease)
rellang.commands = "$$LRELEASE -removeidentical $$_PRO_FILE_"
QMAKE_EXTRA_TARGETS = genlang rellang
PRE_TARGETDEPS = genlang rellang

# Turn off stripping as this causes the install to fail :(
QMAKE_STRIP = echo

unix:!macx {
    target.path = /usr/bin

    translations.path = /usr/share/theheartbeat/translations
    translations.files = translations/*

    desktop.path = /usr/share/applications
    desktop.files = theheartbeat.desktop

    INSTALLS += target translations desktop
}

DISTFILES += \
    theheartbeat.desktop
