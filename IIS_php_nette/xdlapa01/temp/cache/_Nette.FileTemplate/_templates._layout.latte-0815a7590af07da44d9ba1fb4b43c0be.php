<?php //netteCache[01]000381a:2:{s:4:"time";s:21:"0.20101100 1355063011";s:9:"callbacks";a:2:{i:0;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:9:"checkFile";}i:1;s:59:"/home/users/xd/xdlapa01/WWW/IIS/app/templates/@layout.latte";i:2;i:1355061576;}i:1;a:3:{i:0;a:2:{i:0;s:19:"Nette\Caching\Cache";i:1;s:10:"checkConst";}i:1;s:25:"Nette\Framework::REVISION";i:2;s:30:"6a33aa6 released on 2012-10-01";}}}?><?php

// source file: /home/users/xd/xdlapa01/WWW/IIS/app/templates/@layout.latte

?><?php
// prolog Nette\Latte\Macros\CoreMacros
list($_l, $_g) = Nette\Latte\Macros\CoreMacros::initRuntime($template, 'ndkhqxapmg')
;
// prolog Nette\Latte\Macros\UIMacros
//
// block title
//
if (!function_exists($_l->blocks['title'][] = '_lb2a61b2cb96_title')) { function _lb2a61b2cb96_title($_l, $_args) { extract($_args)
;
}}

//
// end of blocks
//

// template extending and snippets support

$_l->extends = empty($template->_extended) && isset($_control) && $_control instanceof Nette\Application\UI\Presenter ? $_control->findLayoutTemplateFile() : NULL; $template->_extended = $_extended = TRUE;


if ($_l->extends) {
	ob_start();

} elseif (!empty($_control->snippetMode)) {
	return Nette\Latte\Macros\UIMacros::renderSnippets($_control, $_l, get_defined_vars());
}

//
// main template
//
?>
<!DOCTYPE html>
<html lang="en">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<meta http-equiv="Content-Language" content="en" />

	<meta name="description" content="IIS Lékárna xdlapa01 a xkebis00" />
<?php if (isset($robots)): ?>	<meta name="robots" content="<?php echo htmlSpecialChars($robots) ?>" />
<?php endif ?>

	<title><?php if ($_l->extends) { ob_end_clean(); return Nette\Latte\Macros\CoreMacros::includeTemplate($_l->extends, get_defined_vars(), $template)->render(); }
ob_start(); call_user_func(reset($_l->blocks['title']), $_l, get_defined_vars()); echo $template->trim($template->striptags(ob_get_clean()))  ?> | Nette example</title>

	<link rel="stylesheet" type="text/css" media="screen" href="<?php echo htmlSpecialChars($basePath) ?>/css/site.css" />
	<script src="http://nette.github.com/resources/js/netteForms.js"></script>
</head>

<body>
<?php $iterations = 0; foreach ($flashes as $flash): ?>	<div class="flash <?php echo htmlSpecialChars($flash->type) ?>
"><?php echo Nette\Templating\Helpers::escapeHtml($flash->message, ENT_NOQUOTES) ?></div>
<?php $iterations++; endforeach ?>

	<div id="content">
<?php Nette\Latte\Macros\UIMacros::callBlock($_l, 'content', $template->getParameters()) ?>
	</div>

<?php if ($user->loggedIn): ?>	<p id="logged-in">Přihlášen <?php echo Nette\Templating\Helpers::escapeHtml($user->identity->name, ENT_NOQUOTES) ?>
(<?php echo Nette\Templating\Helpers::escapeHtml($user->identity->role, ENT_NOQUOTES) ?>
). <a href="<?php echo htmlSpecialChars($_control->link("Sign:out")) ?>">Odhlásit</a></p>
<?php endif ?>
</body>
</html>