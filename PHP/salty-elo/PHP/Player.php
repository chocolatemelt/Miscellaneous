<?php

/**
 * This class contains base player information.
 *
 * @author Kevin Zhang <kzhaaang@gmail.com>
 * @copyright Copyright (c) 2015 Kevin Zhang
 * @license MIT License
 */

class Player {

  // define variables
  protected $elo;
  protected $provisionals;
  protected $lowestElo;
  protected $highestElo;
  protected $numGames;

  /**
   * constructor function
   * $e   = current Elo ranking (default 1400)
   * $p   = provisional games left (default 30)
   */
  public function __construct($e = 1400, $p = 30) {
    $this->elo = $e;
    $this->provisionals = $p;
    $this->lowestElo = $e;
    $this->highestElo = $e;
    $this->numGames = $e;
  }

  /**
   * toString function
   * outputs debug information about the player
   */
  public function __toString() {
    $s = ' | ';
    $e = 'Current Elo: ' . $this->getElo();
    $p = 'Provisionals: ' . $this->getProvisionals();
    $g = 'Games played: ' . $this->getGames();
    $h = 'Highest Elo reached: ' . $this->highestElo();
    $l = 'Lowest Elo reached: ' . $this->lowestElo();
    return $e . $s . $g . $s . $p . $s . $h . $s . $l;
  }

  /**
   * accessor/mutator functions
   */
  public function getElo() { return $this->elo; }
  public function getProvisionals() { return $this->provisionals; }
  public function inProvisionals() { return ($this->provisionals > 0) ? true : false; }
  public function lowestElo() { return $this->lowestElo; }
  public function highestElo() { return $this->highestElo; }
  public function getGames() { return $this->numGames; }

  public function setElo($e) {
    $this->elo = $e;
    if($this->elo > $this->highestElo) {
      $this->highestElo = $this->elo;
    }
    if($this->elo < $this->lowestElo) {
      $this->lowestElo = $this->elo;
    }
  }
  public function setProvisionals($p) { $this->provisionals = $p; }
  public function playGames($n = 1) {
    $this->numGames += $n;
    $this->provisionals -= $n;
    if($this->provisionals < 0) $this->provisionals = 0;
  }

}

?>
