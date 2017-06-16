<?php //netteCache[01]000386a:2:{s:4:"time";s:21:"0.09609000 1354987691";s:9:"callbacks";a:2:{i:0;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:9:"checkFile";}i:1;s:64:"/home/users/xd/xdlapa01/WWW/IIS/app/templates/Emails/email.latte";i:2;i:1354977496;}i:1;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:10:"checkConst";}i:1;s:25:"Nette\Framework::REVISION";i:2;s:30:"6a33aa6 released on 2012-10-01";}}}?><?php

// source file: /home/users/xd/xdlapa01/WWW/IIS/app/templates/Emails/email.latte

?><?php
// prolog Nette\Latte\Macros\CoreMacros
list($_l, $_g) = Nette\Latte\Macros\CoreMacros::initRuntime($template, '6uddkvobrn')
;
// prolog Nette\Latte\Macros\UIMacros

// snippets support
if (!empty($_control->snippetMode)) {
	return Nette\Latte\Macros\UIMacros::renderSnippets($_control, $_l, get_defined_vars());
}

//
// main template
//
$mail->from = "NOREPLY@IISLekarna.cz" ?>
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <title>Výkaz od IIS lékárny</title>
    </head>
    <body>
        <p>Dobrý den,</p>

        <p>Toto je výkaz doplatků za léky pro Vaši pojišťovnu.</p>
        <br />

        <table class="grid">
            <tr>
                <th>Datum a čas</th>
                <th>Kód léku</th>
                <th>Doplatek</th>
                <th>&nbsp;</th>
            </tr>

<?php $celkDopl = 0 ;$iterations = 0; foreach ($arr as $arrProdej): ?>
            <tr>
                <td><?php echo Nette\Templating\Helpers::escapeHtml($prodejRep->findById(key($arr))->datum, ENT_NOQUOTES) ?></td>
                <td></td>
                <td></td>
                <td></td>
            </tr>

<?php $iterations = 0; foreach ($arrProdej as $lekCislo): ?>
            <tr>
                <td></td>
                <td><?php echo Nette\Templating\Helpers::escapeHtml($lekCislo, ENT_NOQUOTES) ?></td>
                <td><?php echo Nette\Templating\Helpers::escapeHtml($lekRep->findByID($lekCislo)->doplatek, ENT_NOQUOTES) ?></td>
                <td></td>
            </tr>
<?php $celkDopl += $lekRep->findByID($lekCislo)->doplatek ;$iterations++; endforeach ;$iterations++; endforeach ?>

            <tr>
                <td></td>
                <td></td>
                <td>Celkem: <?php echo Nette\Templating\Helpers::escapeHtml($celkDopl, ENT_NOQUOTES) ?></td>
                <td></td>
            </tr>
        </table>
    </body>
</html>